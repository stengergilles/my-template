
#include "Layout.h"
#include "imgui_internal.h" // For GImGui
#include <algorithm> // For std::sort, std::max
#include <map>       // For std::map
#include <array>     // For std::array

namespace Layout {

// --- Global Instance ---
CardLayoutManager g_cardLayoutManager;

// --- Card Implementation ---
Card::Card(std::string id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, std::function<void()> content)
    : m_id(std::move(id)), m_width(width), m_height(height), m_hAlign(hAlign), m_vAlign(vAlign), m_content(std::move(content)) {}

void Card::render(const ImVec2& pos, const ImVec2& size) {
    ImGui::SetNextWindowPos(pos);

    ImVec2 setSize = ImVec2(0, 0); // Default to auto-size for ImGui::SetNextWindowSize
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

    // Determine the size to pass to ImGui::SetNextWindowSize.
    // If mode is PERCENTAGE or AUTOFIT, use the calculated size.
    // If mode is CONTENT, pass 0.0f to let ImGui auto-size.
    if (m_width.mode == SizeMode::PERCENTAGE || m_width.mode == SizeMode::AUTOFIT) {
        setSize.x = size.x;
    }
    if (m_height.mode == SizeMode::PERCENTAGE || m_height.mode == SizeMode::AUTOFIT) {
        setSize.y = size.y;
    }

    ImGui::SetNextWindowSize(setSize);

    // Only add AlwaysAutoResize if *any* dimension is CONTENT.
    // This flag tells ImGui to adjust the window size to fit its content.
    // If a dimension is 0 in SetNextWindowSize, and this flag is present,
    // ImGui will calculate that dimension based on content.
    if (m_width.mode == SizeMode::CONTENT || m_height.mode == SizeMode::CONTENT) {
        flags |= ImGuiWindowFlags_AlwaysAutoResize;
    }

    if (ImGui::Begin(m_id.c_str(), nullptr, flags)) {
        if (m_content) {
            m_content();
        }
    }
    ImGui::End();
}

// --- CardLayoutManager Implementation ---
void CardLayoutManager::beginCard(const std::string& id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, std::function<void()> content) {
    m_cards.emplace_back(std::make_unique<Card>(id, width, height, hAlign, vAlign, content));
}

void CardLayoutManager::calculateLayout(const ImVec2& displaySize) {
    // Reset calculated sizes and positions
    for (auto& card : m_cards) {
        card->m_calculatedSize = ImVec2(0, 0);
        card->m_calculatedPos = ImVec2(0, 0);
    }

    // --- Pass 1: Pre-calculate fixed and content sizes for all cards ---
    // This pass measures CONTENT and PERCENTAGE dimensions.
    // AUTOFIT dimensions are left as 0 for now.
    for (auto& card : m_cards) {
        // Calculate width
        if (card->m_width.mode == SizeMode::PERCENTAGE) {
            card->m_calculatedSize.x = displaySize.x * card->m_width.value / 100.0f;
        } else if (card->m_width.mode == SizeMode::CONTENT) {
            // Measure content width by rendering off-screen
            ImGui::SetNextWindowPos(ImVec2(-10000.0f, -10000.0f));
            ImGui::SetNextWindowSize(ImVec2(0,0));
            ImGui::Begin((card->m_id + "_MEASURE_W").c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
            card->m_content();
            ImVec2 measuredSize = ImGui::GetWindowSize();
            ImGui::End();
            card->m_calculatedSize.x = measuredSize.x;
        }

        // Calculate height
        if (card->m_height.mode == SizeMode::PERCENTAGE) {
            card->m_calculatedSize.y = displaySize.y * card->m_height.value / 100.0f;
        } else if (card->m_height.mode == SizeMode::CONTENT) {
            // Measure content height by rendering off-screen
            ImGui::SetNextWindowPos(ImVec2(-10000.0f, -10000.0f));
            ImGui::SetNextWindowSize(ImVec2(0,0));
            ImGui::Begin((card->m_id + "_MEASURE_H").c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
            card->m_content();
            ImVec2 measuredSize = ImGui::GetWindowSize();
            ImGui::End();
            card->m_calculatedSize.y = measuredSize.y;
        }
    }

    // Group cards by vertical alignment (rows)
    std::map<VAlignment, std::vector<Card*>> rows;
    for (auto& card : m_cards) {
        rows[card->m_vAlign].push_back(card.get());
    }

    // --- Pass 2: Determine Row Heights (Vertical Distribution) ---
    float totalFixedAndPercentageHeight = 0.0f;
    int autoFitRowsCount = 0;
    std::map<VAlignment, float> rowMinHeights; // Minimum height required for each row

    // Calculate minimum height for each row based on PERCENTAGE/CONTENT cards
    for (auto const& [vAlign, cardList] : rows) {
        float minHeight = 0.0f;
        bool hasAutoFitHeight = false;
        for (Card* card : cardList) {
            if (card->m_height.mode == SizeMode::PERCENTAGE || card->m_height.mode == SizeMode::CONTENT) {
                minHeight = ImMax(minHeight, card->m_calculatedSize.y);
            } else if (card->m_height.mode == SizeMode::AUTOFIT) {
                hasAutoFitHeight = true;
            }
        }
        rowMinHeights[vAlign] = minHeight;
        if (!hasAutoFitHeight) {
            totalFixedAndPercentageHeight += minHeight;
        } else {
            autoFitRowsCount++;
        }
    }

    float remainingVerticalSpace = displaySize.y - totalFixedAndPercentageHeight;
    float autoFitRowUnitHeight = (autoFitRowsCount > 0) ? remainingVerticalSpace / autoFitRowsCount : 0.0f;

    // Assign final heights to cards
    for (auto const& [vAlign, cardList] : rows) {
        bool hasAutoFitHeight = false;
        for (Card* card : cardList) {
            if (card->m_height.mode == SizeMode::AUTOFIT) {
                hasAutoFitHeight = true;
                break;
            }
        }

        float currentRowHeight = rowMinHeights[vAlign];
        if (hasAutoFitHeight) {
            currentRowHeight = autoFitRowUnitHeight;
        }

        for (Card* card : cardList) {
            if (card->m_height.mode == SizeMode::AUTOFIT) {
                card->m_calculatedSize.y = currentRowHeight;
            }
            // For PERCENTAGE/CONTENT, height is already calculated.
            // If a row has mixed types, all cards in that row will take the max height of the row.
            // This ensures consistent row height.
            card->m_calculatedSize.y = ImMax(card->m_calculatedSize.y, currentRowHeight);
        }
    }

    // --- Pass 3: Determine Column Widths and Positions within Rows (Horizontal Distribution) ---
    float currentY = 0.0f;
    std::array<VAlignment, 3> vAlignOrder = {VAlignment::TOP, VAlignment::CENTER, VAlignment::BOTTOM};

    for (VAlignment vAlignKey : vAlignOrder) {
        if (rows.count(vAlignKey) == 0) continue;

        std::vector<Card*> currentRowCards = rows[vAlignKey];
        std::sort(currentRowCards.begin(), currentRowCards.end(), [](Card* a, Card* b) {
            return static_cast<int>(a->m_hAlign) < static_cast<int>(b->m_hAlign);
        });

        float totalFixedAndPercentageWidthInRow = 0.0f;
        int autoFitWidthCardsInRow = 0;

        // Calculate fixed/percentage widths within this row
        for (Card* card : currentRowCards) {
            if (card->m_width.mode == SizeMode::PERCENTAGE || card->m_width.mode == SizeMode::CONTENT) {
                totalFixedAndPercentageWidthInRow += card->m_calculatedSize.x;
            } else if (card->m_width.mode == SizeMode::AUTOFIT) {
                autoFitWidthCardsInRow++;
            }
        }

        float remainingHorizontalSpaceInRow = displaySize.x - totalFixedAndPercentageWidthInRow;
        float autoFitColumnWidth = (autoFitWidthCardsInRow > 0) ? remainingHorizontalSpaceInRow / autoFitWidthCardsInRow : 0.0f;

        float currentX = 0.0f;
        float maxRowHeight = 0.0f; // Track max height for this row for vertical positioning

        for (Card* card : currentRowCards) {
            if (card->m_width.mode == SizeMode::AUTOFIT) {
                card->m_calculatedSize.x = autoFitColumnWidth;
            }
            card->m_calculatedPos.x = currentX;
            currentX += card->m_calculatedSize.x;
            maxRowHeight = ImMax(maxRowHeight, card->m_calculatedSize.y);
        }

        // Assign vertical position for cards in this row
        for (Card* card : currentRowCards) {
            card->m_calculatedPos.y = currentY;
        }
        currentY += maxRowHeight;
    }

    m_layoutCalculated = true;
}

void CardLayoutManager::renderCards() {
    if (!m_layoutCalculated) return;
    for (const auto& card : m_cards) {
        card->render(card->m_calculatedPos, card->m_calculatedSize);
    }
}

void CardLayoutManager::endCardLayout(const ImVec2& displaySize) {
    calculateLayout(displaySize);
    renderCards();
    m_cards.clear(); // Reset for the next frame
    m_layoutCalculated = false;
}

// --- Public API Implementation ---
void BeginCardLayout() {
    // This function can be used for any setup if needed in the future.
}

void BeginCard(const std::string& id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, const std::function<void()>& content) {
    g_cardLayoutManager.beginCard(id, width, height, hAlign, vAlign, content);
}

void EndCard() {
    // No-op, but part of the declarative API style.
}

void EndCardLayout(const ImVec2& displaySize) {
    g_cardLayoutManager.endCardLayout(displaySize);
}

} // namespace Layout
