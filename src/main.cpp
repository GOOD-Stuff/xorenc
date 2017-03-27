/*
    гаммирование - в нашем случае сложение по модулю 2 (xor)
*/
#include <QCoreApplication>
#include <QString>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <unistd.h>
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
static char *path_encr;

static const QString get_keys(ifstream &file);
static const QString get_text(ifstream &file);
static const QString get_encr_text(const QString key,
                                   const QString clear_text);
static void clear_space(QString &text);

static int menu(int argc, char **argv);
static QString get_encr(ifstream &file);

int main(int argc, char *argv[]){    
    cout << "\tNice to meet you! Let\'s start!" << endl;

    path_key = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_text = (char*)calloc(MAX_INPUT, sizeof(char*));

    // Get path to files
    if( menu(argc, argv) != 0 )
        return -FAILURE;

    ifstream key_file(path_key);
    if( !key_file.is_open()){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    ifstream plain_file(path_text);
    if( !plain_file.is_open()){
        fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    const QString key_str = get_keys(key_file);
    const QString plain_str = get_text(plain_file);
    bool flag;
    printf("%x | %x\n", key_str.at(0), plain_str.at(0));

    printf("%x | %s\n", key_str.toStdWString().at(0) ^ plain_str.toStdWString().at(0),
           key_str.toStdWString().at(0) ^ plain_str.toStdWString().at(0));
    //printf("%x | %s\n", key_str.toUInt( ^ plain_str.at(0),
      //                  key_str.at(0) ^ plain_str.at(0));

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
                  "-h\t- this help view;\n";
    if( ( argc != 5 ) ){
        if( !strcmp(argv[1], "-h") ){
            cout << help << endl;
            return -FAILURE;
        }
        cout << "\tYou doing something wrong!" << endl;
        cout << help << endl;
        return -FAILURE;
    }

    int opt;
    while( (opt = getopt(argc, argv, "k:t:h:")) != -1 ){
        switch(opt){
        case 'k':
            cout << "\tYour key file will be: ";
            strcpy(path_key, optarg);
            cout << path_key << endl;
            break;
        case 't':
            cout << "\tYour encrypted file will be: ";
            strcpy(path_text, optarg);
            cout << path_text << endl;
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
    clear_space(s_key_words);

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
    clear_space(s_text);

    free(text);
    return s_text;
}

/**
 * @brief get_encr_text - Encrypts the plaintext;
 * @param colm_alph     - column of alphabet (actually, it is just row of alphabet);
 * @param key           - Key for encryption;
 * @param clear_text    -
 * @return encr_text    - if all was successfully, string of encrypted text;
 *         NULL         - if was error;
 */
static const QString get_encr_text(const QString key, const QString clear_text){
    QString encr_text;

    for( auto iter_text = clear_text.begin(); iter_text != clear_text.end();
        iter_text++ ){
        //encr_text.push_back();
    }

/*
    int incr_alph, incr_key, incr_text;
    int pos = 0;
    QString::const_iterator iter_key = key.begin();
    for( QString::const_iterator iter_txt = clear_text.begin(); iter_txt != clear_text.end();
                                       iter_txt++, iter_key++, pos++ ) {
        if( iter_key == key.end() )
            iter_key = key.begin();

        incr_alph = colm_alph.indexOf(*iter_txt);
        if( incr_alph < 0 ){
            encr_text.append(*iter_txt);
            iter_key--;
            continue;
        }

kek:
        incr_key = colm_alph.indexOf(*iter_key);
        if( incr_key < 0 ){
            iter_key++;
            if( iter_key == key.end() )
                iter_key = key.begin();
            goto kek;
        }

        incr_text = incr_alph + incr_key;

        if( incr_text >= alph_length )
            incr_text = abs(incr_text - alph_length);

        encr_text.append(colm_alph.at(incr_text));
    }
*/
    return encr_text;
}


static void clear_space(QString &text){
   text = text.toLower();
   int pos = 0;
   while( text.contains(' ') || text.contains('\n') ) {
       pos = text.indexOf(' ');
       if( pos != -1 )
           text.remove(pos, 1);
       pos = text.indexOf('\n');
       if( pos != -1 )
           text.remove(pos, 1);
   }
}
