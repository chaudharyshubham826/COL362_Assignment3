#include <iostream>
#include <cstring>
#include <climits>
#include <fstream>
#include <vector>
#include "file_manager.h"
#include "errors.h"
using namespace std;

void print(FileHandler *fh, FileManager *fm)
{
    PageHandler ph;
    char *data;
    int pagenum = 0;
    int num;

    while (true)
    {
        try
        {
            ph = fh->PageAt(pagenum);
        }
        catch (const InvalidPageException &e)
        {
            cout << "file over" << endl;
            break;
        }
        data = ph.GetData();
        for (int i = 0; i < PAGE_CONTENT_SIZE / 4; i++)
        {
            memcpy(&num, &data[i * 4], sizeof(int));
            // if(num==INT_MIN){
            // 	break;
            // }
            cout << "Pagenum is " << pagenum << " Found at page " << num << " .Offset is " << i << endl;
        }

        cout << endl;
        pagenum += 1;
        fh->FlushPage(pagenum - 1);
        fh->UnpinPage(pagenum - 1);
    }

    
    fh->FlushPages();
}

int main(int argc, const char *argv[])
{
    //reading query_search.txt
    ifstream inputFile;
    inputFile.open(argv[2]);
    if (!inputFile.is_open())
    {
        //cout << "File doesn't exist";
    }
    

    int num;
    string str;
    vector<int> numbers;

    while (inputFile >> str >> num)
    {
        
        numbers.push_back(num);
    }
    //////////////////////////////////All numbers to search in vector numbers now////////////////////////////

    //opening sorted_input
    FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);
    //cout << "File opened having data, sorted_input" << endl;
    //cout << BUFFER_SIZE << " " << PAGE_CONTENT_SIZE << endl;
    



    int first_page_no = fh.FirstPage().GetPageNum();
    int last_page_no = fh.LastPage().GetPageNum();
    //fh.UnpinPage(lastFoundPage);
    //cout << "First page number is " << first_page_no << endl;
    //cout << "Last page number is " << last_page_no << endl;

    PageHandler ph;
    int pagenum = 0;
    char *data;
    ////////////////////////////////
    for(int i = first_page_no; i <= last_page_no; i++){
        ph = fh.PageAt(i);
        data = ph.GetData();
        

        for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
        {
            memcpy(&num, &data[j * 4], sizeof(int));
            
            
           // cout << "page_no: " << i << ", offset: " << j << ", data: " << num<< endl;
            
        }

        fh.UnpinPage(i);
        fh.FlushPages();
    }

    ///////////////////////////////////
    FileHandler outputfh;
    outputfh = fm.CreateFile(argv[3]);
    PageHandler ph2 = outputfh.NewPage();
    char *odata = ph2.GetData();
    ////////////////////////////////////////////
    int offset = 0;
    int opagenum = 0;
    int to_search = 0;
    int low = first_page_no;
    int high = last_page_no;
    int mid = 0;
    int start_no = 0;
    int end_no = 0;









    for(int i = 0; i < numbers.size(); i++){
        to_search = numbers.at(i);
        //cout<<"Searching for: "<<to_search<<endl;

        low = first_page_no;
        high = last_page_no;
        mid = 0;
        start_no = 0;
        end_no = 0;


        while(low <= high){
            mid = (low+high)/2;

            try
            { 
                ph = fh.PageAt(mid);
            }
            catch (const InvalidPageException &e)
            {
                //cout << "file over" << endl;
                break;
            }

            data = ph.GetData();

            memcpy(&start_no, &data[0], sizeof(int));
            memcpy(&end_no, &data[((PAGE_CONTENT_SIZE / 4) - 1) * 4], sizeof(int));


            //////Case1: Last Page
            if(start_no < to_search && end_no == INT_MIN){
                for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                {
                    memcpy(&num, &data[j * 4], sizeof(int));
                    // cout << "Page: " << pagenum << ", Data: " << num << endl;

                    if (num == to_search)
                    {
                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            //cout<<j;
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            //cout<<"I am here"<<endl;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            offset = 1;
                        }

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            //cout<<j<<endl;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                       // cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    }
                }
                fh.UnpinPage(mid);
                fh.FlushPages();

                break;
            }
            

            else if(start_no > to_search){
                high = mid-1;
                fh.UnpinPage(mid);
                fh.FlushPages();
                continue;
            }

            else if(end_no < to_search){
                low = mid+1;
                fh.UnpinPage(mid);
                fh.FlushPages();
                continue;
            }

            else if(start_no < to_search && end_no > to_search){
                for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                {
                    memcpy(&num, &data[j * 4], sizeof(int));
                    // cout << "Page: " << pagenum << ", Data: " << num << endl;

                    if (num == to_search)
                    {
                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            //cout<<j;
                            offset++;
                        }

                        else{
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            //cout<<"I am here"<<endl;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            offset = 1;
                        }

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            //cout<<j<<endl;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    }
                }
                fh.UnpinPage(mid);
                fh.FlushPages();

                low = INT_MAX;
            }


            else if(start_no == to_search && (end_no > to_search || end_no == INT_MIN)){
                for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                {
                    memcpy(&num, &data[j * 4], sizeof(int));
                    // cout << "Page: " << pagenum << ", Data: " << num << endl;

                    if (num == to_search)
                    {

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            //cout<<j;
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            //cout<<"I am here"<<endl;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            offset = 1;
                        }

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            //cout<<j<<endl;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }
                        //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    }
                }

                fh.UnpinPage(mid);
                fh.FlushPages();

                
                while (true)
                {
                    mid--;

                    try
                    { //cout<<"I am here";
                        ph = fh.PageAt(mid);
                        //fh.UnpinPage(pagenum);
                    }
                    catch (const InvalidPageException &e)
                    {
                        //cout << "file over" << endl;
                        low = INT_MAX;
                        break;
                    }

                    data = ph.GetData();

                    for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                    {
                        memcpy(&num, &data[j * 4], sizeof(int));
                        // cout << "Page: " << pagenum << ", Data: " << num << endl;

                        if (num == to_search)
                        {
                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                //cout<<j;
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                //cout<<"I am here"<<endl;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                offset = 1;
                            }

                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                //cout<<j<<endl;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }
                          //  cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                        }
                    }

                    memcpy(&start_no, &data[0], sizeof(int));

                    fh.UnpinPage(mid);
                    fh.FlushPages();

                    if(start_no < to_search){
                        low = INT_MAX;
                        break;
                    }

                }
                // fh.UnpinPage(mid);
                // fh.FlushPages();
                
                
            }


            else if (end_no == to_search && start_no < to_search)
            {
                for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                {
                    memcpy(&num, &data[j * 4], sizeof(int));
                    // cout << "Page: " << pagenum << ", Data: " << num << endl;

                    if (num == to_search)
                    {

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            //cout<<j;
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            //cout<<"I am here"<<endl;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            offset = 1;
                        }

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            //cout<<j<<endl;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }
                        //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    }
                }
                fh.UnpinPage(mid);
                fh.FlushPages();

                while (true)
                {
                    mid++;



                    try
                    { //cout<<"I am here";
                        ph = fh.PageAt(mid);
                        //fh.UnpinPage(pagenum);
                    }
                    catch (const InvalidPageException &e)
                    {
                       // cout << "file over" << endl;
                        low = INT_MAX;
                        break;
                    }

                    data = ph.GetData();

                    for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                    {
                        memcpy(&num, &data[j * 4], sizeof(int));
                        // cout << "Page: " << pagenum << ", Data: " << num << endl;

                        if (num == to_search)
                        {
                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                //cout<<j;
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                //cout<<"I am here"<<endl;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                offset = 1;
                            }

                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                //cout<<j<<endl;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }
                            //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                        }
                    }

                    memcpy(&end_no, &data[((PAGE_CONTENT_SIZE / 4) - 1) * 4], sizeof(int));
                    fh.UnpinPage(mid);
                    fh.FlushPages();
                    if (end_no > to_search)
                    {
                        low = INT_MAX;
                        break;
                    }
                    
                }
                // fh.UnpinPage(mid);
                // fh.FlushPages();
            }


            else if(end_no == to_search && start_no == to_search){
                for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                {
                    memcpy(&num, &data[j * 4], sizeof(int));
                    // cout << "Page: " << pagenum << ", Data: " << num << endl;

                    if (num == to_search)
                    {
                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            //cout<<j;
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            //cout<<"I am here"<<endl;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[offset * 4], &mid, sizeof(int));
                            offset = 1;
                        }

                        if (offset < PAGE_CONTENT_SIZE / 4)
                        {
                            //cout<<j<<endl;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }

                        else
                        {
                            outputfh.FlushPage(opagenum);
                            outputfh.UnpinPage(opagenum);
                            outputfh.FlushPages();
                            //fm.ClearBuffer();
                            opagenum++;
                            ph2 = outputfh.NewPage();
                            odata = ph2.GetData();
                            offset = 0;
                            memcpy(&odata[(offset)*4], &j, sizeof(int));
                            offset++;
                        }
                        //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    }
                }
                fh.UnpinPage(mid);
                fh.FlushPages();

                int var = mid;

                while (true)
                {
                    var--;

                    try
                    { //cout<<"I am here";
                        ph = fh.PageAt(var);
                        //fh.UnpinPage(pagenum);
                    }
                    catch (const InvalidPageException &e)
                    {
                        //cout << "file over" << endl;
                        low = INT_MAX;
                        break;
                    }

                    data = ph.GetData();

                    for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                    {
                        memcpy(&num, &data[j * 4], sizeof(int));
                        // cout << "Page: " << pagenum << ", Data: " << num << endl;

                        if (num == to_search)
                        {
                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                memcpy(&odata[offset * 4], &var, sizeof(int));
                                //cout<<j;
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                //cout<<"I am here"<<endl;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[offset * 4], &var, sizeof(int));
                                offset = 1;
                            }

                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                //cout<<j<<endl;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }
                            //cout << "TO_FILE= PAGE: " << var << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                        }
                    }

                    memcpy(&start_no, &data[0], sizeof(int));

                    fh.UnpinPage(var);
                    fh.FlushPages();

                    if (start_no < to_search)
                    {
                        low = INT_MAX;
                        break;
                    }
                    
                }
                // fh.UnpinPage(var);
                // fh.FlushPages();

                while (true)
                {
                    mid++;

                    try
                    { //cout<<"I am here";
                        ph = fh.PageAt(mid);
                        //fh.UnpinPage(pagenum);
                    }
                    catch (const InvalidPageException &e)
                    {
                        //cout << "file over" << endl;
                        low = INT_MAX;
                        break;
                    }

                    data = ph.GetData();

                    for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                    {
                        memcpy(&num, &data[j * 4], sizeof(int));
                        // cout << "Page: " << pagenum << ", Data: " << num << endl;

                        if (num == to_search)
                        {
                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                //cout<<j;
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                //cout<<"I am here"<<endl;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[offset * 4], &mid, sizeof(int));
                                offset = 1;
                            }

                            if (offset < PAGE_CONTENT_SIZE / 4)
                            {
                                //cout<<j<<endl;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }

                            else
                            {
                                outputfh.FlushPage(opagenum);
                                outputfh.UnpinPage(opagenum);
                                outputfh.FlushPages();
                                //fm.ClearBuffer();
                                opagenum++;
                                ph2 = outputfh.NewPage();
                                odata = ph2.GetData();
                                offset = 0;
                                memcpy(&odata[(offset)*4], &j, sizeof(int));
                                offset++;
                            }
                            //cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                        }
                    }

                    memcpy(&end_no, &data[((PAGE_CONTENT_SIZE / 4) - 1) * 4], sizeof(int));

                    fh.UnpinPage(mid);
                    fh.FlushPages();

                    if (end_no > to_search)
                    {
                        low = INT_MAX;
                        break;
                    }
                    
                }
                // fh.UnpinPage(mid);
                // fh.FlushPages();
            }


        }
        int end = -1;
        if (offset < PAGE_CONTENT_SIZE / 4)
        {
            memcpy(&odata[offset * 4], &end, sizeof(int));
            //cout<<j;
            offset++;
        }

        else
        {
            outputfh.FlushPage(opagenum);
            outputfh.UnpinPage(opagenum);
            outputfh.FlushPages();
            fm.ClearBuffer();
            //cout<<"I am here"<<endl;
            opagenum++;
            ph2 = outputfh.NewPage();
            odata = ph2.GetData();
            offset = 0;
            memcpy(&odata[offset * 4], &end, sizeof(int));
            offset = 1;
        }

        if (offset < PAGE_CONTENT_SIZE / 4)
        {
            memcpy(&odata[offset * 4], &end, sizeof(int));
            //cout<<j;
            offset++;
        }

        else
        {
            outputfh.FlushPage(opagenum);
            outputfh.UnpinPage(opagenum);
            outputfh.FlushPages();
            fm.ClearBuffer();
            //cout<<"I am here"<<endl;
            opagenum++;
            ph2 = outputfh.NewPage();
            odata = ph2.GetData();
            offset = 0;
            memcpy(&odata[offset * 4], &end, sizeof(int));
            offset = 1;
        }
    }

    int end = INT_MIN;
    while (offset < PAGE_CONTENT_SIZE / 4)
    {

        memcpy(&odata[offset * 4], &end, sizeof(int));
        offset++;
    }
     

    fm.CloseFile(fh);
    fm.CloseFile(outputfh);

     //cout << "Output file" << endl;
     //fh = fm.OpenFile(argv[3]);
     //print(&fh, &fm);
    //fm.DestroyFile(argv[3]);

    return 0;
    
}
