
#include <signal.h>

#include "testwindefs.h"

#include "lc3.h"
#include "mappedfile.h"

using namespace pcore;

uint8_t keyStates[256]{};
int keyCode = 0;
int keyChar = 0;

//
UINT getVChar(unsigned char vkey)
{
    UINT charCode = ::MapVirtualKey(vkey, MAPVK_VK_TO_CHAR);
    return charCode;
}

int onVMLoop(void *ptr, uint16_t instr)
{
    //fprintf(stdout, "onVMLoop \n");
    //fflush(stdout);

    lc3vm *vm = (lc3vm *)ptr;

    //lc3_vm_inject_key(vm, 'y');
    //return 0;
    
    // check keystate, and inject character
    // if there is one
    //::GetAsyncKeyState(vKey);
    memset(keyStates, 0, sizeof(keyStates));

    ::GetKeyboardState(keyStates);

    // Figure out which keys are characters or not
    // and return the first one that says it is pressed
    for (int i=0;i<256;i++)
    {
        // If it's not convertible to a character
        // then just continue the loop
        unsigned char vChar = getVChar(i);
        if (vChar == 0) continue;

        //fprintf(stdout, "%c ", vChar);
        //fflush(stdout);

        // high order bit indicates key is pressed
        if (keyStates[i]&0x80) {
            //lc3_vm_inject_key(vm, vChar);
            return 0;
        }
    }
    printf("\n");

    return 0;
}

// Console IO
// In order to use the console IO routines effectively, we need
// to disable the buffering and echoing of the console input.
// During shutdown, we can turn the buffering back on.
static DWORD old_console_in_mode;

static void config_io() PC_NOEXCEPT_C
{
    // set the console mode to no buffering
    // and no echo
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms686033(v=vs.85).aspx

    // Set the input terminal mode
    ::GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &old_console_in_mode);
    //DWORD mode = old_console_in_mode & ~ENABLE_ECHO_INPUT & ~ENABLE_LINE_INPUT;
    ::SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT |ENABLE_VIRTUAL_TERMINAL_INPUT );
    
    // Set the output terminal
    ::SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT| ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    ::FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

static void restore_input_buffering() PC_NOEXCEPT_C
{
    ::SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), old_console_in_mode);
}

static int check_key() PC_NOEXCEPT_C
{
    return ::WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 1000) == WAIT_OBJECT_0 && _kbhit();
}

void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}


void test_lc3_core(bspan &fspan)
{
    printf("==== test_lc3_core ====\n");

    lc3vm vm;
    lc3_vm_init(&vm);
    lc3_vm_set_checkkey(&vm, check_key);
    //vm.fCheckKey = check_key;
    //vm.fLoopFun = onVMLoop;

    lc3_load_image_span(&vm, &fspan);

    lc3_vm_exec(&vm);
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("usage: test_lc3 filename.obj \n");
        return 0;
    }

    signal(SIGINT, handle_interrupt);
    config_io();

    auto mfile = MappedFile::create_shared(argv[1]);
    bspan fspan;
    bspan_init_from_data(&fspan, mfile->data(), mfile->size());
    test_lc3_core(fspan);

    restore_input_buffering();
}

