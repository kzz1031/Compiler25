#include <iostream>
#include <string>
#include <set>
#include <map>
#include "color.hh"
#include "tinyxml2.hh"

void Color::print() {
    cout << "Function: " << funcname << endl;
    cout << "Color Map:" << endl;
    for (const auto& pair : color_map) {
        cout << "Temp " << pair.first << ": Color " << pair.second << endl;
    }
    cout << "Spills:" << endl;
    for (const auto& spill : spills) {
        cout << "Temp " << spill << endl;
    }
}

void ColorMap::print() {
    cout << "Color Maps for all functions:" << endl;
    for (const auto& pair : color_map) {
        pair.second->print();
    }
}

ColorMap* xml2colormap(string filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        cerr << "Error loading XML file: " << filename << endl;
        return nullptr;
    }
    XMLElement *root = doc.RootElement();
    if (!root || strcmp(root->Name(), "COLORING") != 0) {
        cerr << "Invalid XML format: missing root COLORING element" << endl;
        return nullptr;
    }

    ColorMap *colormap = new ColorMap();

    XMLElement *coloring = root ->FirstChildElement("Coloring");
    // Get function name (ignore the k value, no use for now)
    while (coloring != nullptr) {
        const char* func = coloring->Attribute("func");
        if (func == nullptr) {
            cerr << "Error: Missing func attribute in Coloring element." << endl;
            return nullptr;
        }
        // Create a new Color object
        Color *color = new Color(func);
        colormap->addColor(func, color);
        XMLElement *colors_e = coloring->FirstChildElement("Colors");
        if (colors_e != nullptr) {
            XMLElement *color_e = colors_e->FirstChildElement("Color");
            while (color_e != nullptr) {
                int node, color_value;
                if (color_e->QueryIntAttribute("node", &node) == XML_SUCCESS &&
                    color_e->QueryIntAttribute("color", &color_value) == XML_SUCCESS) {
                    color->add_color(node, color_value);
                }
                color_e = color_e->NextSiblingElement("Color");
            }
        }
        XMLElement *spills_e = colors_e->NextSiblingElement("Spills");
        if (spills_e != nullptr) {
            XMLElement *spill_e = spills_e->FirstChildElement("Spill");
            while (spill_e != nullptr) {
                int node;
                if (spill_e->QueryIntAttribute("node", &node) == XML_SUCCESS) {
                    color->add_spill(node);
                }
                spill_e = spill_e->NextSiblingElement("Spill");
            }
        }
        coloring = coloring->NextSiblingElement("Coloring");
    }
    return colormap;
}