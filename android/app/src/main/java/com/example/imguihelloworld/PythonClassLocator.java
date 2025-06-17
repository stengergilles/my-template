package com.example.imguihelloworld;

import com.chaquo.python.Python;
import com.chaquo.python.PyObject;
import com.chaquo.python.PyException;

public class PythonClassLocator {

    public static PyObject instantiatePythonClass(String className,Python Py, Object... args) {
        // Split the class name into module and class parts
        String[] parts = className.split("\\.");
        if (parts.length < 2) {
            throw new IllegalArgumentException("Invalid class name format. Use 'mod.submod.class'");
        }

        String moduleName = String.join(".", java.util.Arrays.copyOf(parts, parts.length - 1));
        String classSimpleName = parts[parts.length - 1];

        try {
            // Import the module
            PyObject module = Py.getModule(moduleName);
            // Get the class from the module
            PyObject pyClass = module.get(classSimpleName);
            // Instantiate the class with the provided arguments
            return pyClass.call(args);
        } catch (PyException e) {
            // Handle exceptions from Python
            throw new RuntimeException("Error instantiating Python class: " + e.getMessage(), e);
        }
    }
}

