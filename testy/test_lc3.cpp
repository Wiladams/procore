
#include "testwindefs.h"

#include "lc3.h"
#include "mappedfile.h"

using namespace pcore;

uint8_t keyStates[256]{};
int keyCode = 0;
int keyChar = 0;


void onVMLoop(void *ptr, uint16_t instr)
{
    //fprintf(stdout, "onVMLoop \n");
    //fflush(stdout);

    lc3vm *vm = (lc3vm *)ptr;

    lc3_vm_inject_key(vm, 'y');

    return ;
    
    // check keystate, and inject character
    // if there is one
    //::GetAsyncKeyState(vKey);
    memset(keyStates, 0, sizeof(keyStates));

    ::GetKeyboardState(keyStates);
    // If any of keyStates != 0
    // then inject that one key into the vm
    for (int i=0;i<256;i++)
    {
        fprintf(stdout, "%2x ", keyStates[i]);
        fflush(stdout);

        if (keyStates[i]&0x80)
            lc3_vm_inject_key(vm, i);
    }
    printf("\n");
}

void test_lc3_core(bspan &fspan)
{
    lc3vm vm;
    lc3_vm_init(&vm);
    vm.fLoopFun = onVMLoop;

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

    auto mfile = MappedFile::create_shared(argv[1]);
    bspan fspan;
    bspan_init_from_data(&fspan, mfile->data(), mfile->size());
    test_lc3_core(fspan);
}

