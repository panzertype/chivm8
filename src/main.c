#include "app.c"

int main(int argc, char **argv) {
    App app;    

    AppInit(&app);
    AppHandleCliArgs(&app, argc, argv);
    AppLoadRomIntoChip8(&app);
    AppCreateWindow(&app);

    while (AppShouldRun(&app)) {
        AppProcessChip8Inputs(&app);
        AppRunChip8(&app);
        AppDrawChip8DisplayBuffer(&app);
        AppDraw(&app);
    }

    AppDestroy(&app);

    return 0;
}
