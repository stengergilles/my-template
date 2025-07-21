
#pragma once

#include "imgui.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace Layout {

enum class SizeMode {
    CONTENT,    // Size to content
    PERCENTAGE, // Percentage of the parent container
    AUTOFIT,    // Stretch to fill available space
};

struct Dimension {
    SizeMode mode = SizeMode::CONTENT;
    float value = 0.0f;
};

enum class HAlignment {
    LEFT,
    CENTER,
    RIGHT
};

enum class VAlignment {
    TOP,
    CENTER,
    BOTTOM
};

// Forward declaration
class CardLayoutManager;

// Represents a single card to be rendered
class Card {
public:
    Card(std::string id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, std::function<void()> content);
    void render(const ImVec2& pos, const ImVec2& size);

private:
    friend class CardLayoutManager;

    std::string m_id;
    Dimension m_width;
    Dimension m_height;
    HAlignment m_hAlign;
    VAlignment m_vAlign;
    std::function<void()> m_content;

    // Calculated values
    ImVec2 m_calculatedSize;
    ImVec2 m_calculatedPos;
};

// Manages the layout process
class CardLayoutManager {
public:
    void beginCard(const std::string& id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, std::function<void()> content);
    void endCardLayout(const ImVec2& displaySize);

private:
    void calculateLayout(const ImVec2& displaySize);
    void renderCards();

    std::vector<std::unique_ptr<Card>> m_cards;
    bool m_layoutCalculated = false;
};

// Global layout manager instance
extern CardLayoutManager g_cardLayoutManager;

// Public API functions
void BeginCardLayout();
void BeginCard(const std::string& id, Dimension width, Dimension height, HAlignment hAlign, VAlignment vAlign, const std::function<void()>& content);
void EndCard(); // This is now a placeholder, but good for API consistency
void EndCardLayout(const ImVec2& displaySize);

} // namespace Layout
