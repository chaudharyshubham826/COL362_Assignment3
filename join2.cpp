#include <iostream>
#include <cstring>
#include <climits>
#include <fstream>
#include <vector>
#include "file_manager.h"
#include "errors.h"
using namespace std;

bool debugprintinsert=true;

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
            cout<<"file over"<<endl;
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
	//reading query_search.txt

	//opening sorted_input
	FileManager fm;
	FileHandler fh_unsorted = fm.OpenFile(argv[1]);
    PageHandler ph;
    FileHandler fh_sorted = fm.OpenFile(argv[2]);
    //FileHandler check = fm.OpenFile("./TestCases/TC_join2/output_join2");
    //print(&check,&fm);
	cout << "File opened" << endl;
	cout<<BUFFER_SIZE<<" "<<PAGE_CONTENT_SIZE<<endl;
	

	
	//sort(numbers.begin(), numbers.end());

    //fh.UnpinPage(lastFoundPage);
	/*cout << "First page number is " << lastFoundPage << endl;
    
	cout<<"hello";*/
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
            ph = fh_unsorted.PageAt(pagenum);
        }
        catch(const InvalidPageException& e)
        {
            cout<<"file over"<<endl;
            break;
        }  
        data=ph.GetData();

        for(int i=0;i<PAGE_CONTENT_SIZE/4;i++){
            
            int search=0;
            memcpy(&search,&data[i*4],sizeof(int));
            num=0;
            int pagenum2=0;
            PageHandler ph2;
            char*data2;

            while(true){
            try
            {   //cout<<"I am here";
                ph2 = fh_sorted.PageAt(pagenum2);
                //fh.UnpinPage(pagenum);
            }
            catch(const InvalidPageException& e)
            {
                q = -1;
                cout<<"file over"<<endl;
                break;
            }  
            data2=ph2.GetData();
            //fh.FlushPage(pagenum);



            for(int i=0;i<PAGE_CONTENT_SIZE/4;i++){
                    memcpy(&num,&data2[i*4],sizeof(int));
                    if(num==INT_MIN){
                        break;
                    }
                    if(search==num){
                    //cout<<"j is : "<<j<<" ";
                    q = pagenum2;
                    offset = i;
                    break;
                    }
                    

            }
            fh_sorted.UnpinPage(pagenum2);
            fh_sorted.FlushPages();
            if(q == pagenum2){
                break;
            }
            pagenum2+=1;
            
        }
        if(q==-1){
            continue;
        }
        else{
            while(true){
                
                //cout<<q<<" "<<offset<<endl;
                try
                {   //cout<<"I am here";
                    ph2 = fh_sorted.PageAt(q);
                    //fh.UnpinPage(pagenum);
                }
                catch(const InvalidPageException& e)
                {
                    cout<<"file over"<<endl;
                    break;
                } 
                data2 = ph2.GetData();
                if(data2[offset*4]==search){
                    //cout<<search<<endl;
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
                    
                }
                if(offset == (PAGE_CONTENT_SIZE/4)-1 ){
                    offset = 0;
                    q = q+1;
                }
                else{
                    offset += 1;
                }
                 fh_sorted.UnpinPage(q);
                fh_sorted.FlushPages();
            } 
        }  
        }
        outputfh.FlushPage(opagenum);
        fh_unsorted.FlushPage(pagenum);
        pagenum++;
        
    }
    print(&outputfh,&fm);
    fm.DestroyFile(argv[3]);
}