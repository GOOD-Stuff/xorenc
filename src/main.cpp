/*
    гаммирование - в нашем случае сложение по модулю 2 (xor)
*/
#include <QCoreApplication>
#include <QString>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <qalgorithms.h>
#include <string>
#include <stdio.h>
#include <sys/stat.h>

#define SUCCESS 0
#define FAILURE 1

using namespace std;
static char *path_key;
static char *path_text;
static char *path_alph;
static char *path_encr;
static int alph_length = 33;
static bool isKeyNumb = false;
static bool isTxtNumb = false;

/*************** PROTOTYPES ****************/
static const QString get_keys(ifstream &file);
static const QString get_text(ifstream &file);
static const QString get_alph(ifstream &file);

static const QString get_encr_text(const QString alph, const QString key,
                                   const QString clear_text);
static void clear_enters(QString &text);
static int menu(int argc, char **argv);
/******************************************/


const QString get_plai_text(const QString alph, const QString key,
                                   const QString clear_text){
    QString plain_text;

    int indx_alph, indx_key, incr_text;
    for( auto iter_text = clear_text.begin(), iter_key = key.begin();
                                            iter_text != clear_text.end();
                                                    iter_text++, iter_key++ ){

        if( iter_key == key.end())
            iter_key = key.begin();

        // If string contains numbers, then work only with numbers
        //if( isTxtNumb == false ){
            indx_alph = alph.indexOf(*iter_text);
            if( indx_alph < 0 ){
                plain_text.append(*iter_text);
                iter_key--;
                continue;
            }
            indx_alph++;
        /* }
        else{
            QString tmp_str;
            while( !iter_text->isSpace() && iter_text->isNumber() ){
                tmp_str.append(*iter_text++);
            }
            indx_alph = atoi(tmp_str.toStdString().c_str());
        }
*/
        if( isKeyNumb == false ){
            indx_key = alph.indexOf(*iter_key);
            if( indx_key < 0 ){
                iter_text--;
                continue;
            }
            indx_key++;
        }
        else{
            QString tmp_str;
            while( !iter_key->isSpace() && iter_key->isNumber() ){
                tmp_str.append(*iter_key++);
            }
            indx_key = atoi(tmp_str.toStdString().c_str());
        }

        incr_text = indx_alph ^ indx_key;

        if( incr_text > 0 )
            incr_text--;

        if( incr_text > alph_length )
            incr_text = incr_text % alph_length;

        plain_text.append(alph.at(incr_text));
    }

    return plain_text;

}


int main(int argc, char *argv[]){    
    cout << "\tNice to meet you! Let\'s start!" << endl;

    path_key = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_text = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_alph = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_encr = (char*)calloc(MAX_INPUT, sizeof(char*));


    // Get path to files
    if( menu(argc, argv) != 0 )
        return -FAILURE;

    ifstream key_file(path_key);
    if( !key_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    ifstream plain_file(path_text);
    if( !plain_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " plaintext: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    ifstream alph_file(path_alph);
    if( !alph_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " alphabet: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    const QString key_str = get_keys(key_file);
    const QString plain_str = get_text(plain_file);
    const QString alph_str = get_alph(alph_file);
    const QString encr_str = get_encr_text(alph_str, key_str, plain_str);
    const QString denc_str = get_plai_text(alph_str, key_str, encr_str);

    cout << "Your alph:\t\t" << alph_str.toStdString() << endl;
    cout << "Your key:\t\t" << key_str.toStdString() << endl;
    cout << "Your plain text:\t" << plain_str.toStdString() << endl;
    cout << "Your encr text:\t\t" << encr_str.toStdString() << endl;
    cout << "Your decr text:\t\t" << denc_str.toStdString() << endl;

    if( path_encr[0] == (char)NULL )
        strcpy(path_encr, "encr.txt");
    ofstream encr_file(path_encr, ios_base::out | ios_base::trunc);
    if( !encr_file.is_open() ){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        key_file.close();
        plain_file.close();

        return -FAILURE;

    }

    encr_file.write(encr_str.toUtf8(), encr_str.toUtf8().length());

    key_file.close();
    plain_file.close();
    alph_file.close();
    encr_file.close();

    free(path_key);
    free(path_text);
    free(path_alph);
    free(path_encr);

    return SUCCESS;
}

/**
 * @brief menu - Parse input arguments on keys and path to file;
 * @param argc - Count of input arguments;
 * @param argv - Input arguments;
 * @return SUCCESS - If all arguments is normal;
 *         FAILURE - If was error;
 */
static int menu(int argc, char **argv){
    string help = "\tYou must to use:\n"
                  "-k\t- file which contains key;\n"
                  "-t\t- file which contains plain text;\n"
                  "-a\t- file which contains alphabet;\n"
                  "-e\t- file where will be contains encrypted text;\n"
                  "-h\t- this help view;\n";
    if( ( argc < 6 ) || ( argc > 8 ) ){
        cout << "\tYou doing something wrong!" << endl;
        cout << help << endl;
        return -FAILURE;
    }

    int opt;
    while( (opt = getopt(argc, argv, "k:t:e:a:h:")) != -1 ){
        switch(opt){
        case 'k':
            cout << "\tYour key file will be: ";
            strcpy(path_key, optarg);
            cout << path_key << endl;
            break;
        case 't':
            cout << "\tYour plaintext file will be: ";
            strcpy(path_text, optarg);
            cout << path_text << endl;
            break;
        case 'a':
            cout << "\tYour alphabet file will be: ";
            strcpy(path_alph, optarg);
            cout << path_alph << endl;
            break;
        case 'e':
            cout << "\tYour encrypted file will be: ";
            strcpy(path_encr, optarg);
            cout << path_encr << endl;
            break;
        case 'h':
            cout << help << endl;
            return -FAILURE;
        default:
            cout << "\tYou wroooong! Why?" << endl;
            cout << help << endl;
            return -FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * @brief get_key - return key words or phrase for encryption, length of alphabet
 * @param file    - file which contains key words
 * @return s_key_words - if all was successfully, string of key phrase;
 *         NULL        - if was error;
 */
static const QString get_keys(ifstream &file){
    struct stat info_file;
    stat(path_key, &info_file);  // get size of file

    if( info_file.st_size <= 0 ){
        printf("Why your %s is empty? (^o^)\r\n", path_key);
        return NULL;
    }

    char *key_words = (char*)calloc(info_file.st_size, sizeof(char*));
    if( key_words == NULL ){
        printf("Couldn't allocate memory for key words. Sorry (X_X)\r\n");
        return NULL;
    }

    file.read(key_words, info_file.st_size);
    if( !file ) {
        printf("Couldn't read file with keys. Sorry (*-*)\r\n");
        return NULL;
    }

    QString s_key_words(key_words);  
    QRegExp regul("\\d+");
    if( s_key_words.contains(regul) )
        isKeyNumb = true;

    free(key_words);

    return s_key_words;
}

/**
 * @brief get_text - return string of text for encryption
 * @param file     - file which contains not encrypted text
 * @return s_text - if all was successfully, string of not encrypted text for encryption;
 *         NULL        - if was error;
 */
static const QString get_text(ifstream &file){
    struct stat info_file;
    stat(path_text, &info_file);

    if( info_file.st_size <= 0 ){   // Check size of file
        printf("Why your %s is empty? (^o^)\r\n", path_text);
        return NULL;
    }

    char *text = (char*) calloc(info_file.st_size, sizeof(char*));
    if( text == NULL ){     // If we can't allocate memory
        printf("Couldn't allocate memory for plaintext. Sorry (X_X)\r\n");
        return NULL;
    }

    file.seekg(0, ios_base::beg);           // Set cursor to start of file
    file.read(text, info_file.st_size);
    if( !file ){
        printf("Couldn't read file with plaintext. Sorry (*-*)\r\n");
        return NULL;
    }

    QString s_text(text);  
    QRegExp regul("\\d+");
    if( s_text.contains(regul) )
        isTxtNumb = true;

    free(text);
    return s_text;
}

/**
 * @brief get_alph     - return the string of alphabet for encryption;
 * @param file         - file which contains alphabet;
 * @return s_alph_line - if all was successfully, string of alphabet;
 *         NULL        - if was error;
 */
static const QString get_alph(ifstream &file){
    struct stat info_file;
    stat(path_alph, &info_file);

    if( info_file.st_size <= 0 ){   // Check the size of file
        printf("Why your %s is empty? (^o^)\r\n", path_alph);
        return NULL;
    }

    file.seekg(0, ios_base::beg); // Set position into start of file
    alph_length = info_file.st_size;

    char *alph_line = (char*) calloc(alph_length, sizeof(char*));
    if( alph_line == NULL ){
        printf("Couldn't allocate memory for alphabet. Sorry (X_X)\r\n");
        return NULL;
    }

    file.read(alph_line, alph_length);
    if( !file ){
        printf("Couldn't read file with alphabet. Sorry (*-*)\r\n");
        return NULL;
    }

    QString s_alph_line(alph_line);
    clear_enters(s_alph_line);

    alph_length = s_alph_line.size();

    free(alph_line);
    return s_alph_line;
}

/**
 * @brief get_encr_text - Encrypts the plaintext;
 * @param colm_alph     - column of alphabet (actually, it is just row of alphabet);
 * @param key           - Key for encryption;
 * @param clear_text    -
 * @return encr_text    - if all was successfully, string of encrypted text;
 *         NULL         - if was error;
 */
static const QString get_encr_text(const QString alph, const QString key,
                                   const QString clear_text){
    QString encr_text;

    int indx_alph, indx_key, incr_text;
    for( auto iter_text = clear_text.begin(), iter_key = key.begin();
                                            iter_text != clear_text.end();
                                                    iter_text++, iter_key++ ){

        if( iter_key == key.end())
            iter_key = key.begin();

        // If string contains numbers, then work only with numbers
        //if( isTxtNumb == false ){
            indx_alph = alph.indexOf(*iter_text);
            if( indx_alph < 0 ){
                encr_text.append(*iter_text);
                iter_key--;
                continue;
            }
            indx_alph++;
        /* }
        else{
            QString tmp_str;
            while( !iter_text->isSpace() && iter_text->isNumber() ){
                tmp_str.append(*iter_text++);
            }
            indx_alph = atoi(tmp_str.toStdString().c_str());
        }
*/
        if( isKeyNumb == false ){
            indx_key = alph.indexOf(*iter_key);
            if( indx_key < 0 ){
                iter_text--;
                continue;
            }
            indx_key++;
        }
        else{
            QString tmp_str;
            while( !iter_key->isSpace() && iter_key->isNumber() ){
                tmp_str.append(*iter_key++);
            }
            indx_key = atoi(tmp_str.toStdString().c_str());
        }        

        incr_text = indx_alph ^ indx_key;

        if( incr_text > 0 )
            incr_text--;

        if( incr_text > alph_length )
            incr_text = incr_text % alph_length;

        encr_text.append(alph.at(incr_text));
    }

    return encr_text;
}



/**
 * @brief clear_enters - Clear string only from \n symbols;
 * @param text         - String of text (key, alphabet, plain text);
 */
static void clear_enters(QString &text){
    text = text.toLower();
    int pos = 0;
    while( text.contains('\n') ) {
        pos = text.indexOf('\n');
        if( pos != -1 )
            text.remove(pos, 1);
    }
}
