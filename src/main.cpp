/*
    гаммирование - в нашем случае сложение по модулю 2 (xor)
*/
#include <QCoreApplication>
#include <QString>
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QFile>
#include <unistd.h>
#include <QByteArray>
#include <QByteArrayData>
#include <algorithm>
#include <qalgorithms.h>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SUCCESS 0
#define FAILURE 1

using namespace std;
static char *path_key;
static char *path_text;
static char *path_encr;

/*************** PROTOTYPES ****************/
static const QString get_keys(ifstream &file);
static const QString get_text(ifstream &file);
static const QString get_encr_text(const QString key,
                                   const QString clear_text);
static const QString get_cst_text(int fd);
static void clear_enters(QString &text);
static const QByteArray set_buff(const QString encr_str);
static int menu(int argc, char **argv);
/******************************************/

int main(int argc, char *argv[]){    
    cout << "\tNice to meet you! Let\'s start!" << endl;

    path_key = (char*)calloc(MAX_INPUT, sizeof(char*));
    path_text = (char*)calloc(MAX_INPUT, sizeof(char*));
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
                        " options: %s\r\n", strerror(errno));
        return -FAILURE;
    }

    const QString key_str = get_keys(key_file);
    const QString plain_str = get_text(plain_file);
    const QString encr_str = get_encr_text(key_str, plain_str);    

    cout << "Your key:\t\t" << key_str.toStdString() << endl;
    cout << "Your plain text:\t" << plain_str.toStdString() << endl;
    cout << "Your encr text:\t\t" << encr_str.toStdString() << endl;    


    if( path_encr[0] == (char)NULL )
        strcpy(path_encr, "encr.txt");

    QFile encr_file(path_encr);
    encr_file.open(QIODevice::ReadWrite | QIODevice::Truncate);
            //int fd = open(path_encr, O_CREAT | O_RDWR);

    if( !encr_file.isOpen() /*fd < 0 ) { //  !encr_file.is_open() */){
         fprintf(stderr, "Can't open file with"
                        " options: %s\r\n", strerror(errno));
       key_file.close();
        plain_file.close();

        return -FAILURE;

    }

    QByteArray byte_str = set_buff(encr_str);
    int ret = encr_file.write(byte_str);
    if( ret < 0 ){
        fprintf(stderr, "Can't write data into file: %s\r\n", strerror(errno));
        key_file.close();
        plain_file.close();
        encr_file.close();
        return -FAILURE;
    }

    encr_file.close();

    // Block of decryption
    int fd = open(path_encr, O_RDONLY);
    const QString deen_str = get_cst_text(fd);
    cout << "Your txt for decrypt:\t" << deen_str.toStdString() << endl;
    const QString lel = get_encr_text(key_str, deen_str);
    cout << "Your decrypt txt:\t\t" << lel.toStdString() << endl;

    key_file.close();
    plain_file.close();


    free(path_key);
    free(path_text);
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
                  "-e\t- file where will be contains encrypted text;\n"
                  "-h\t- this help view;\n";
    if( ( argc < 5 ) || ( argc > 7 ) ){
        cout << "\tYou doing something wrong!" << endl;
        cout << help << endl;
        return -FAILURE;
    }

    int opt;
    while( (opt = getopt(argc, argv, "k:t:e:h:")) != -1 ){
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
    clear_enters(s_key_words);

    free(key_words);

    return s_key_words;
}

static const QString get_cst_text(int fd){
    struct stat info_file;
    stat(path_text, &info_file);

    if( info_file.st_size <= 0 ){   // Check size of file
        printf("Why your %s is empty? (^o^)\r\n", path_text);
        return NULL;
    }
    cout << info_file.st_size << endl;
    char *text = (char*) calloc(info_file.st_size, sizeof(char*));
    if( text == NULL ){     // If we can't allocate memory
        printf("Couldn't allocate memory for plaintext. Sorry (X_X)\r\n");
        return NULL;
    }

    read(fd, text, info_file.st_size);
    QString s_text(text);

    free(text);
    return s_text;
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
    clear_enters(s_text);

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
    uint16_t hex_key;
    uint16_t hex_pln;
    uint16_t hex_enc;

    for( auto iter_text = clear_text.begin(), iter_key = key.begin();
            iter_text != clear_text.end();
                iter_text++, iter_key++ ){
        if( iter_key == key.end())
            iter_key = key.begin();
        hex_key = iter_key->unicode();
        hex_pln = iter_text->unicode();
        hex_enc = hex_key ^ hex_pln;

        encr_text.push_back(hex_enc);
    }

    return encr_text;
}

/**
 * @brief set_buff - Fill array of bytes values from encr_str;
 * @param encr_str - string with text (bytes);
 * @return array of bytes;
 */
static const QByteArray set_buff(const QString encr_str){
    QByteArray result;

    quint8 tmp_byte_safe = 0;
    for( int i = 0; i < encr_str.toUtf8().size(); i++ ){
        tmp_byte_safe = encr_str.toUtf8().at(i);
        result.append(tmp_byte_safe);
    }

    return result;
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
