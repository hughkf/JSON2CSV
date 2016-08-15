/* 
 * File:   Converter.h
 * Author: hugh
 *
 * Created on July 11, 2016, 6:09 PM
 */

#ifndef CONVERTER_H
#define CONVERTER_H

#include "Node.h"
#include "SchemaTree.h"

using namespace std;
using namespace boost::algorithm;
using json = nlohmann::json;

class Converter {
    
private:
    map<string, deque<string>> embeddedColumns;
    Node* header;
    string delimiter, replaceStr;
    SchemaTree* h;

public:

    Converter(string delimiter, Node* header, SchemaTree* h)
    {
        this->delimiter = delimiter;
        this->header = header;
        this->h = h;
    }

    static int getMaxList(map<string,deque<string>> ras)
    {
        size_t max = 0;
        deque<string> ra;
        map<string,deque<string>>::iterator it;
        for (it = ras.begin(); it != ras.end(); ++it) 
        {
            ra = ras[it->first];
            if (ra[0].substr(0,1) == Main::internalDelimiterString())
                ra = Main::split(ra[0].substr(1, ra[0].size()-2), 
                        Main::internalDelimiterChar());
            if (max < ra.size())
                max = ra.size();
        }
        return max;
    }
    
    Node* searchHeaderTree(Node* rootNode, string searchPath)
    {
        searchPath = boost::trim_copy(searchPath);
        if (searchPath.length() == 0 || searchPath == "_")
            return rootNode;
        deque<Node*> schemaNodes;
        this->h->getHeaderPaths(rootNode, schemaNodes);
        assert (schemaNodes.size() > 0);
        
        deque<Node*>::iterator it;
        for (it = schemaNodes.begin(); it != schemaNodes.end(); ++it)
        {
            if (*it != NULL && (*it)->getPath() == searchPath)
            {
                return *it;
            }
        }
        cout << "search for " << searchPath << " failed!";
        return NULL;
    }
        
    void simpleAppend(string key, string value)
    {
        std::ofstream out;
        string filepath = Main::getResultsDirectory() + "/" + key + ".txt";
        out.open(filepath, std::ios::app);
        out << value + "\n";
        out.close();
    }

    void convert(json node, deque<string> currentPath) 
    {
        string pathString = "_" + join(currentPath, "_");
        Node* schemaNode = this->searchHeaderTree(header, pathString);
        assert (schemaNode != NULL);
        if (node.is_object())
        {            
            LinkedList<Node>* en = schemaNode->getChildren();
            while (en != NULL && !en->is_null())
            {
                Node* child = (Node*)en->data();
                json value = node[ child->getName() ];
                if (child->getChildren()->getSize() == 0 && (value == NULL 
                    || value.is_null()))
                {                    
                    this->simpleAppend(child->getPath(), "\"null\"");
                    
                } else if (value.is_primitive())
                {
                    this->simpleAppend(child->getPath(), "\"" + value.dump() + "\"");

                } else {
                    currentPath.push_back(child->getName());
                    this->convert(value, currentPath);
                    if (currentPath.size() > 0) currentPath.pop_back();
                }
                en = en->nextNode();
            }
        } else if (node.is_array()) 
        {
            for (json item : node) 
            {
                if (item.is_primitive())
                {
                    embeddedColumns[pathString].push_back("\"" + item.dump() + "\"");
                
                } else {
                    this->convert(item, currentPath);
                } 
            }
        }
    }

    map<string,deque<string>> getEmbeddedColumns()
    {
        return this->embeddedColumns;
    }
    
    ~Converter()
    { } // see ~SchemaTree & ~Node
};

#endif /* CONVERTER_H */