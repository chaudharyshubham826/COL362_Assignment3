#include <iostream>
#include <cstring>
#include <climits>
#include <fstream>
#include <vector>
#include "file_manager.h"
#include "errors.h"
using namespace std;

int main(int argc, const char *argv[])
{
    ifstream inputFile;
    inputFile.open(argv[2]);
    if (!inputFile.is_open())
    {
        cout << "File doesn't exist";
    }
    cout << "Input file for integers taken" << endl;

    int num;
    string str;
    vector<int> numbers;

    while (inputFile >> str >> num)
    {
        cout << "Read number " << num << " from the input query_search.txt file" << endl;
        numbers.push_back(num);
    }

    //opening sorted_input
    FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);
    cout << "File opened having data, sorted_input" << endl;
    cout << BUFFER_SIZE << " " << PAGE_CONTENT_SIZE << endl;

    int first_page_no = fh.FirstPage().GetPageNum();
    int last_page_no = fh.LastPage().GetPageNum();
    //fh.UnpinPage(lastFoundPage);
    cout << "First page number is " << first_page_no << endl;
    cout << "Last page number is " << last_page_no << endl;

    PageHandler ph;
    // int pagenum = 0;
    char *data;
    PageHandler ph2;
    char *data2;

    ////////////////////////////////
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

    /////// global vars
    int to_search;
    
    int low = first_page_no;
    int high = last_page_no;
    int mid = 0;
    int start_no = 0;
    int end_no = 0;
    bool found;
    int page = 0;
    int page_off = 0;

    //////////// STEP 1: FINDING USING BINARY SEARCH /////////////////////
    for(int test = 0; test < numbers.size(); test++){
        to_search = numbers.at(test);

        cout << "Searching for first occurence of: " << to_search << endl;

        low = first_page_no;
        high = last_page_no;
        mid = 0;
        start_no = 0;
        end_no = 0;
        found = false;

        while(low <= high){
            mid = (low+high)/2;

            try {ph = fh.PageAt(mid);}
            catch (const InvalidPageException &e){
                cout << "file over" << endl;
                break;
            }

            data = ph.GetData();

            memcpy(&start_no, &data[0], sizeof(int));
            memcpy(&end_no, &data[((PAGE_CONTENT_SIZE / 4) - 1) * 4], sizeof(int));

            for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
            {
                memcpy(&num, &data[j * 4], sizeof(int));
                // cout << "Page: " << pagenum << ", Data: " << num << endl;

                if (num == to_search)
                {
                    found = true;
                    page = mid;
                    page_off = j;
                    cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                    break;
                }
            }

            fh.UnpinPage(mid);
            fh.FlushPages();

            if(found == true && page_off == 0){
                
                while(true){
                    mid--;
                    try {ph = fh.PageAt(mid);}
                    catch (const InvalidPageException &e){
                        cout << "file over" << endl;
                        break;
                    }

                    data = ph.GetData();

                    memcpy(&start_no, &data[0], sizeof(int));
                    memcpy(&end_no, &data[((PAGE_CONTENT_SIZE / 4) - 1) * 4], sizeof(int));
                    for (int j = 0; j < PAGE_CONTENT_SIZE / 4; j++)
                    {
                        memcpy(&num, &data[j * 4], sizeof(int));
                        // cout << "Page: " << pagenum << ", Data: " << num << endl;

                        if (num == to_search)
                        {
                            found = true;
                            page = mid;
                            page_off = j;
                            cout << "TO_FILE= PAGE: " << mid << ", OFFSET: " << j << ", DATA: " << num << " .................." << endl;
                            break;
                        }
                    }
                    fh.UnpinPage(mid);
                    fh.FlushPages();

                    if(start_no != to_search){
                        break;
                    }
                }
                
            }

            if(found == true){
                break;
            }

            if(found != true){
                if(start_no > to_search){
                    high = mid-1;
                }

                else if(end_no < to_search){
                    low = mid+1;
                }

                else {
                    break;
                }
            }
        }


        ////////////////// STEP 2: FINDING LAST OCCURENCE USING LINEAR SEARCH AND STEP 3//////////////////////
        int q = page;
        int q_off = page_off;
        if(found == true){
            
            found = false;

            while(q<=last_page_no && found == false){

                try {ph = fh.PageAt(q);}
                catch (const InvalidPageException &e){
                    cout << "Invalid error, check your code!" << endl;
                    exit(1);
                }
                data = ph.GetData();

                while(q_off < PAGE_CONTENT_SIZE/4){
                    if (data[q_off*4] != to_search)
                    {
                        found = true;
                        break;
                    }
                    q_off++;
                }
                fh.UnpinPage(q);
                fh.FlushPages();

                if(found == true){
                    break;
                }

                q++;
                q_off = 0;
                
            }

            cout<<"Last occurence: "<<"Page: "<<q<<", Offset: "<<q_off<<endl;

        }


        ///////////// STEP 3: PDF ALGO ///////////////////


        if(found == true){

            if(q != page){
            
                try {ph = fh.PageAt(q);
                }
                catch (const InvalidPageException &e){
                    cout << "Invalid error, check your code!" << endl;
                    exit(1);
                }
                data = ph.GetData();

            }

            // fh.UnpinPage(q);
            // fh.FlushPages();
    
            try
            {
                ph2 = fh.PageAt(page);
                fh.MarkDirty(page);
            }
            catch (const InvalidPageException &e)
            {
                cout << "Invalid error, check your code! 2" << endl;
                exit(1);
            }

            data2 = ph2.GetData();
            

            
            int var = 0;

            while (q <= 6){
                var++;
                cout<<"iter "<<var<<endl;
                cout<<page<<q<<endl;

                if(q == page){
                
                    memcpy(&data2[page_off * 4], &data2[q_off * 4], sizeof(int));
                    
                    
                }
                else{
                    memcpy(&data2[page_off * 4], &data[q_off * 4], sizeof(int));
                }
                cout<<"DONE"<<endl;

                
                
                

                if (page_off == PAGE_CONTENT_SIZE / 4 - 1){
                    //go to next page
                    fh.UnpinPage(page);
                    fh.FlushPage(page);

                    
                    page++;

                    if(page != q){

                    
                        try{
                            ph2 = fh.PageAt(page);
                            fh.MarkDirty(page);
                        }
                        catch (const InvalidPageException &e){
                            cout << "Invalid error, check your code! 3" << endl;
                            exit(1);
                        }
                        data2 = ph2.GetData();
                    }

                    else{
                        fh.UnpinPage(q);
                        fh.FlushPages();

                        try{
                            ph2 = fh.PageAt(page);
                            fh.MarkDirty(page);
                        }
                        catch (const InvalidPageException &e){
                            cout << "Invalid error, check your code! 3" << endl;
                            exit(1);
                        }
                        data2 = ph2.GetData();
                    }
                    

                    page_off = 0;
                }

                else
                {
                    page_off++;
                }

                if (q_off == PAGE_CONTENT_SIZE / 4 - 1)
                {
                    //go to next page
                    fh.UnpinPage(q);
                    fh.FlushPages();

                    if(q != page){
                        cout << "q= " << q + 1 << endl;
                        try
                        {
                            ph = fh.PageAt(q + 1);
                        }
                        catch (const InvalidPageException &e)
                        {
                            cout << "Shifting done, q finished!" << endl;
                            break;
                        }

                        data = ph.GetData();
                        
                    }

                    else{
                        ph2 = fh.PageAt(page);
                        fh.MarkDirty(page);
                        data2 = ph2.GetData();

                        try
                        {
                            ph = fh.PageAt(q + 1);
                        }
                        catch (const InvalidPageException &e)
                        {
                            cout << "Shifting done, q finished!" << endl;
                            break;
                        }

                        data = ph.GetData();
                    }

                    q++;
                    q_off = 0;
                }

                else
                {
                    q_off++;
                }

                
            }

            cout<<"Done copying"<<endl;



            // Fill with int min's
            int end = INT_MIN;
            while (page_off < PAGE_CONTENT_SIZE / 4){

                memcpy(&data2[page_off * 4], &end, sizeof(int));
                page_off++;
            }

            fh.UnpinPage(page);
            fh.FlushPage(page);
            fh.UnpinPage(q);
            fh.FlushPage(q);

            //Delete last empty pages
            page++;
            while(page <= last_page_no){
                fh.DisposePage(page);
                fh.FlushPage(page);
                page++;
            }
            fh.FlushPages();
            }
    }

    return 0;
}