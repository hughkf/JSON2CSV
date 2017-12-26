/* 
 * File:   main.cpp
 * Author: hugh
 *
 * Created on July 3, 2016, 10:48 AM
 */

#include "Main.h"
#include "SchemaTree.h"
#include "Converter.h"
#include "Straightener.h"

#ifndef STRRSPN
const char* strrspn( const char* s, const char* delimiters )
{
  const char* p = strchr( s, '\0' );
  while (p-- != s)
    if (!strchr( delimiters, *p ))
      break;
  return p + 1;
}
#endif

inline char* trim_left_advance( char* s, const char* delimiters ) { return s + strspn( s, delimiters );}

Main::Main(){}

void Main::usage(char* prog) 
{
    std::cout << "USAGE: " << prog << " <input filepath> <delimiter> <node>\n";
}

const string Main::getResultsDirectory()
{
    return "TEMP";
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
        perror(fpath);

    return rv;
}

int rmrf(char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

void Main::deleteResultsDirectory()
{
#ifdef _WIN32
    SHFILEOPSTRUCT fileOperation;
    fileOperation.wFunc = FO_DELETE;
    fileOperation.pFrom = Main::getResultsDirectory().c_str();
    fileOperation.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION;

    int result = ::SHFileOperation(&fileOperation);
    if(result != 0) 
    {
        throw std::runtime_error("Could not delete results directory");
    }
#endif
#ifndef _WIN32
    /* std::stringstream s;
    s << "rm -rf " << Main::getResultsDirectory().c_str();
    system(s.str().c_str()); */
 
    rmrf((char *)Main::getResultsDirectory().c_str());

#endif
}

void Main::createResultsDirectory()
{
#ifdef _WIN32
    mkdir(Main::getResultsDirectory().c_str());
#endif
#ifndef _WIN32
    /* std::stringstream s;
    s << "mkdir -p " << Main::getResultsDirectory().c_str();
    system(s.str().c_str()); */

    struct stat st = {0};

    if (stat(Main::getResultsDirectory().c_str(), &st) == -1) {
        mkdir(Main::getResultsDirectory().c_str(), 0755);
    }

#endif
}

const string Main::join(deque<string> strs, string delimiter)
{
    std::stringstream  s;
    std::copy(strs.begin(),strs.end(), std::ostream_iterator<string>(s, delimiter.c_str()));
    return s.str();
}

void Main::mainDriver(char* inputFilePath, char* delimiter, char* nodeKey)
{
    //generate header
    SchemaTree* h = new SchemaTree(delimiter);

    //get JSON text
    string jsonText = h-> SchemaTree::readJSONString(inputFilePath);

    json rootJson = json::parse(jsonText);
    nodeKey = trim_left_advance(nodeKey, " \f\n\r\t" );

    //if argv[3] is whitespace, process the entire JSON
    if ((nodeKey != NULL) && (nodeKey[0] != '\0'))
        rootJson = rootJson[nodeKey];
    
    //get header fields
    h->constructHeader(rootJson);
    
    //convert
//    mkdir(Main::getResultsDirectory().c_str());
    Converter* con = new Converter(delimiter, h->getRootNode(), h);
    deque<string> v = deque<string>();
    con->convert(rootJson, v);
    v.erase(v.begin(), v.end());
    
    //print CSV header
    deque<string> headers = h->getHeader();
    map<string, deque<string>> embeddedStrings = con->getEmbeddedColumns();
    
    delete h;
    delete con;
    
    deque<string>::iterator header;
    for(header = headers.begin(); header != headers.end(); ++header)
        cout << "," << (*header).substr(1);
    cout << "," << endl;

    //print CSV values
    Straightener* s = new Straightener(embeddedStrings, headers, delimiter);
    s->straighten();

    delete s;
    headers.erase(headers.begin(), headers.end());
}

deque<string> Main::split(string str, char delimiter) 
{
    deque<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;
    
    while(getline(ss, tok, delimiter)) 
      internal.push_back(tok);
    
    return internal;
}

const char Main::internalDelimiterChar() {
    return '@';
}

const string Main::internalDelimiterString() {
    return "@";
}

void Main::writeStringToFile(string s, string filePath)
{
    std::ofstream out(filePath);
    out << s;
    out.flush();
    out.close();    
}

/**
 * Debugging utility
 * @param v
 */
void Main::display_deque(const deque<string> &v)
{
    std::copy(v.begin(), v.end(),
        std::ostream_iterator<string>(std::cout, ";"));
}    

int main(int argc, char** argv) 
{
    Main* m = new Main();
    if (argc != 4) 
    {
        m->usage(argv[0]);
        exit(1);
    }
    try {
        Main::createResultsDirectory();
        m->mainDriver(argv[1], argv[2], trim_left_advance(argv[3], " \f\n\r\t"));
        Main::deleteResultsDirectory();
    
    } catch (exception& e) {
        cout << e.what() << endl;
    }
    delete m;
    return 0;
}
