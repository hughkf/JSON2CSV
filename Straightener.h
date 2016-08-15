/* 
 * File:   Straightener.h
 * Author: hugh
 *
 * Created on July 25, 2016, 10:51 PM
 */

#ifndef STRAIGHTENER_H
#define STRAIGHTENER_H

#include <set>
#include "Converter.h"
#include "Main.h"

using namespace std;

class Straightener {
    
private:
    map<string, size_t> columnSizes;
    size_t total;
    size_t maxColumnSize;
    map<string, size_t> duplicatesMap;
    map<string, size_t> indices;
    deque<string> header;
    string delimiter;
    map<string, deque<string>> embeddedStrings;
    
public:
    
    Straightener(map<string, deque<string>> embeddedStrings, deque<string> header, 
        string delimiter)
    {
        this->embeddedStrings = embeddedStrings;
        this->delimiter = delimiter;
        this->header = header;
        this->getColumnSizes();
        this->maxColumnSize = this->getMaxColumnSize();
        this->total = this->totalSize();
        this->getDuplicates();
    }

    void getDuplicates()
    {
        deque<string>::iterator it;
        for (it = header.begin(); it != header.end(); ++it) 
        {
            this->duplicatesMap[*it] = (maxColumnSize == this->columnSizes[*it])?
                                        2 : maxColumnSize;
        }
    }
    
    void getColumnSizes()
    {
        deque<string>::iterator key;
        string filePath;
        int number_of_lines; 
        for (key = header.begin(); key != header.end(); ++key) 
        {
            filePath = Main::getResultsDirectory() + "/" + (*key) + ".txt";
            std::string line;
            std::ifstream myfile(filePath);
            std::getline(myfile, line);
            number_of_lines = 1;
            if (this->embeddedStrings.find(*key) != this->embeddedStrings.end()) 
            {
                number_of_lines = this->embeddedStrings[*key].size();

            } else {
                while (std::getline(myfile, line))
                    ++number_of_lines;
            }
            myfile.close();
            this->columnSizes[(*key)] = number_of_lines;
        }
    }

    long totalSize()
    {
        std::set<size_t> m;
        int size;
        for (deque<string>::iterator it = header.begin(); it != header.end(); ++it) 
        {
            size = this->columnSizes[(*it)];
            m.insert(size);
        }
        long total = 1;
        for (std::set<size_t>::iterator it = m.begin(); it != m.end(); ++it){
            total *= (*it);
        }
        return total;
    }

    /**
     * @return 
     */
    long getMaxColumnSize()
    {
        size_t max = 0;
        for (deque<string>::iterator it = header.begin(); it != header.end(); ++it) 
        {
            if (this->columnSizes[(*it)] > max)
            {
                max = this->columnSizes[(*it)];
            }
        }
        return max;
    }

    string oracle(string key, size_t row)
    {
        string s = this->embeddedStrings[key][this->indices[key]];
        if ((row + 1) % this->duplicatesMap[key] == 0)
            this->indices[key] = (this->indices[key] + 1) % this->columnSizes[key];
        return s;
    }
    
    void straighten() 
    {
        string fileName;
        string line;
        map<string, std::ifstream> files;
        size_t row = 0;
        deque<string>::iterator key;
        //initialization for this method
        for (key = this->header.begin(); key != this->header.end(); ++key) 
        {
            this->indices[*key] = 0;
            fileName = Main::getResultsDirectory() + "/" + (*key) + ".txt";
            files[*key].open(fileName, ios::app);
        }
        for (size_t blockNum = 0; blockNum < this->total; blockNum++)
        {
            cout << row << this->delimiter;
            for (key = this->header.begin(); key != this->header.end(); ++key)
            {
                if (this->embeddedStrings.find(*key) != this->embeddedStrings.end())
                {
                    line = this->oracle(*key, row);
                    
                } else if (files[*key].eof() || !std::getline(files[*key], line) 
                    || line.length() == 0)
                {
                        files[*key].clear();
                        files[*key].seekg(0, ios::beg);                        
                        std::getline(files[*key], line);
                }
                line = (line.length() > 2)? line.substr(1, line.length()-2) : "null";
                cout << line << this->delimiter;
            }
            cout << endl;
            row++;
        }
        for (key = this->header.begin(); key != this->header.end(); ++key) 
            files[*key].close();
    }
    
    ~Straightener()
    {
    } 
};

#endif /* STRAIGHTENER_H */