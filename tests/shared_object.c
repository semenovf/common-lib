/*extern "C" */
#if defined(_WIN32) || defined(_WIN64)
__declspec(dllexport)
#endif
int dl_only_for_testing_purpose (void)
{
    int i = 1233;
    ++i;
    return i;
}