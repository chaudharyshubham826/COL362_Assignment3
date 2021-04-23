#include <iostream>
#include <cstring>
#include <climits>
#include <fstream>
#include <vector>
#include "file_manager.h"
#include "errors.h"
using namespace std;

bool debug=false;

//This function to check the contents of a file(page by page)
void print(FileHandler* fh, FileManager* fm){
	PageHandler ph;
	char* data;
	int pagenum=0;
	int num;

    while(true){
        try
        {
            ph = fh->PageAt(pagenum);
        }
        catch(const InvalidPageException& e)
        {
            if(debug)cout<<"file over"<<endl;
            break;
        }  
        data=ph.GetData();
		for(int i=0;i<PAGE_CONTENT_SIZE/4;i++){
				memcpy(&num,&data[i*4],sizeof(int));
				// if(num==INT_MIN){
				// 	break;
				// }
				cout<<"Pagenum is "<<pagenum<< " Found at page "<<num<<" .Offset is "<<i<<endl;
			}
    
		cout<<endl;
		pagenum+=1;
		fh->FlushPage(pagenum-1);
        fh->UnpinPage(pagenum-1);
    }

	//Flush the pages, close the file and destroy it
	fh->FlushPages();

}
int main(int argc, const char* argv[]) {

    FileManager fm;
	FileHandler fh_unsorted1 = fm.OpenFile(argv[1]);
    PageHandler ph;
    FileHandler fh_unsorted2= fm.OpenFile(argv[2]);

    if(debug)cout << "File opened" << endl;
	if(debug)cout<<BUFFER_SIZE<<" "<<PAGE_CONTENT_SIZE<<endl;

    FileHandler outputfh;
	outputfh=fm.CreateFile(argv[3]);
    PageHandler ph3 = outputfh.NewPage ();
	char *odata = ph3.GetData ();
    char* data;
	int j=0;
	int opagenum=0;
    int pagenum=0;
    int num;
    int q;
    int offset;


    while(true){
        try{
            ph = fh_unsorted1.PageAt(pagenum);
        }
        catch(const InvalidPageException& e)
        {
            if(debug)cout<<"file over"<<endl;
            break;
        }  
        data=ph.GetData();

        for(int i=0; i<PAGE_CONTENT_SIZE/4;i++){
            int search=0;
            memcpy(&search,&data[i*4],sizeof(int));
            num=0;
            int pagenum2=0;
            PageHandler ph2;
            char*data2;

            while(true){
                try{
                    ph2=fh_unsorted2.PageAt(pagenum2);
                }
                catch(const InvalidPageException& e){
                    q = -1;
                    if(debug)cout<<"file over"<<endl;
                    break;
                }
                data2=ph2.GetData();

                for(int i=0;i<PAGE_CONTENT_SIZE/4;i++){
                    memcpy(&num,&data2[i*4],sizeof(int));
                    if(num==INT_MIN){
                        break;
                    }
                    if(search==num){
                     //cout<<"num:"<<num<<" ";
                     if(j==PAGE_CONTENT_SIZE/4){
                        j=0;
                        outputfh.FlushPage(opagenum);
					    outputfh.UnpinPage(opagenum);
					    outputfh.FlushPages();
					    ph3 = outputfh.NewPage ();
					    odata = ph3.GetData ();
                        //opagenum+=1;
                    }
                    memcpy(&odata[j*4], &search,sizeof(int));
                    j+=1;

                //     if(j<PAGE_CONTENT_SIZE/4){
                //         memcpy (&odata[j*4], &num, sizeof(int));
                //     }
                //     else{
                //         // outputfh.FlushPage(opagenum);
                //         outputfh.UnpinPage(opagenum);
                //         outputfh.FlushPages();
                //         opagenum++;
                //         ph3 = outputfh.NewPage ();
                //         odata = ph3.GetData ();
                //         j=-1;
                //         }
                //     j++;
                //     if(j==PAGE_CONTENT_SIZE/4){
				// 	j=0;
				// 	// outputfh.FlushPage(opagenum);
				// 	outputfh.UnpinPage(opagenum);
				// 	outputfh.FlushPages();
				// 	//fm.ClearBuffer();
				// 	opagenum++;
				// 	ph3 = outputfh.NewPage ();
				// 	odata = ph3.GetData ();
				// }

                    }
                    

                    
                    }
                    
              fh_unsorted2.UnpinPage(pagenum2);
              fh_unsorted2.FlushPages();    
              pagenum2++;
            }
            
        }
        fh_unsorted2.UnpinPage(pagenum);
        fh_unsorted1.FlushPages();
        pagenum++;
    }
    int end=INT_MIN;
    while(j<PAGE_CONTENT_SIZE/4){
      
	  memcpy (&odata[j*4], &end, sizeof(int));
	  j++;
	}
    print(&outputfh,&fm);
    fm.CloseFile(fh_unsorted1);
    fm.CloseFile(fh_unsorted2);
    fm.CloseFile(outputfh);

    if(debug)cout<<"|****************begin*********************|"<<endl;
    if(debug) { 
               FileHandler fh=fm.OpenFile("TestCases/TC_join1/output_join1");
               print(&fh, &fm);
    }
    if(debug)cout<<"|***************end**********************|"<<endl;


    fm.DestroyFile(argv[3]);
}

