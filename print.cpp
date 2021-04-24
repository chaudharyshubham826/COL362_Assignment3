#include <iostream>
#include <cstring>
#include <climits>
#include <fstream>
#include <vector>
#include "file_manager.h"
#include "errors.h"
using namespace std;

int main(int argc, const char *argv[]){
    FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);

    PageHandler ph;
    // int pagenum = 0;
    char *data;
    int num = 0;

    int first_page_no = fh.FirstPage().GetPageNum();
    int last_page_no = fh.LastPage().GetPageNum();

    for (int i = first_page_no; i <= last_page_no; i++)
    {
        ph = fh.PageAt(i);
        data = ph.GetData();

        for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
        {
            memcpy(&num, &data[j * 4], sizeof(int));
            // cout << "Page: " << pagenum << ", Data: " << num << endl;

            cout << "page_no: " << i << ", offset: " << j << ", data: " << num << endl;
        }

        fh.UnpinPage(i);
        fh.FlushPages();
    }

    fm.CloseFile(fh);

    return 0;
}