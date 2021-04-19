//Sample file for students to get their code running

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
	ifstream inputFile;
	inputFile.open(argv[2]);
	if(!inputFile.is_open()){
		cout<<"File doesn't exist";
	}
	cout << "Input file for integers taken" << endl;

	//opening sorted_input
	FileManager fm;
	FileHandler fh = fm.OpenFile(argv[1]);
	cout << "File opened" << endl;
	cout<<BUFFER_SIZE<<" "<<PAGE_CONTENT_SIZE<<endl;
	int  num;
	string str;
	vector<int> numbers;

	while (inputFile >> str >> num) {
		cout << "Read number " << num << endl;
		numbers.push_back(num);
	}
	//sort(numbers.begin(), numbers.end());


    int lastFoundPage = fh.FirstPage().GetPageNum();
    //fh.UnpinPage(lastFoundPage);
	cout << "First page number is " << lastFoundPage << endl;
    
	cout<<"hello";
	FileHandler outputfh;
	outputfh=fm.CreateFile(argv[3]);
    PageHandler ph2 = outputfh.NewPage ();
	char *odata = ph2.GetData ();
	int j=0;
	int opagenum=0;

	//cout<<"Hrllo";
	for (int i = 0; i < numbers.size(); i++) {
        int search=numbers.at(i);
		num=0;
		int pagenum=0;
		PageHandler ph;
		char*data;

		while(true){
        try
        {   //cout<<"I am here";
            ph = fh.PageAt(pagenum);
			//fh.UnpinPage(pagenum);
        }
        catch(const InvalidPageException& e)
        {
            cout<<"file over"<<endl;
            break;
        }  
        data=ph.GetData();
		//fh.FlushPage(pagenum);



		for(int i=0;i<PAGE_CONTENT_SIZE/4;i++){
				memcpy(&num,&data[i*4],sizeof(int));
				if(num==INT_MIN){
					break;
				}
				if(search==num){
				//cout<<"j is : "<<j<<" ";
				if(j<PAGE_CONTENT_SIZE/4){
				memcpy (&odata[j*4], &pagenum, sizeof(int));
				//cout<<j;
				}
				else{
                  outputfh.FlushPage(opagenum);
				  outputfh.UnpinPage(opagenum);
				   outputfh.FlushPages();
				  //fm.ClearBuffer();
				  opagenum++;
				  //cout<<"I am here"<<endl;
				  ph2 = outputfh.NewPage ();
		          odata = ph2.GetData ();
				  j=-1;
				}
				j++;
				if(j<PAGE_CONTENT_SIZE/4){
				//cout<<j<<endl;
				memcpy (&odata[(j)*4], &i, sizeof(int));
				}
				else{
                  outputfh.FlushPage(opagenum);
				  outputfh.UnpinPage(opagenum);
				  outputfh.FlushPages();
				  //fm.ClearBuffer();
				  opagenum++;
				  ph2 = outputfh.NewPage ();
		          odata = ph2.GetData ();
				  j=-1;
				}
				j++;
				if(j==PAGE_CONTENT_SIZE/4){
					j=0;
					outputfh.FlushPage(opagenum);
					outputfh.UnpinPage(opagenum);
					outputfh.FlushPages();
					//fm.ClearBuffer();
					opagenum++;
					ph2 = outputfh.NewPage ();
					odata = ph2.GetData ();
				}
				cout<<"Pagenum is "<<pagenum<< " Number is "<<num<<" .Offset is "<<i<<endl;
				}
			}

        
		pagenum+=1;
        fh.UnpinPage(pagenum-1);
		fh.FlushPages();
    }



	cout<<"Iteration "<<i<<"PagenumOutput: "<<opagenum<<"j is : "<<j<<endl;
	int end=-1;
    if(j<PAGE_CONTENT_SIZE/4){
				memcpy (&odata[j*4], &end, sizeof(int));
				//cout<<j;
				}
				else{
                  outputfh.FlushPage(opagenum);
				  outputfh.UnpinPage(opagenum);
				  outputfh.FlushPages();
				  fm.ClearBuffer();
				  //cout<<"I am here"<<endl;
				  opagenum++;
				  ph2 = outputfh.NewPage ();
		          odata = ph2.GetData ();
				  j=-1;
				}
				j++;
				if(j<PAGE_CONTENT_SIZE/4){
				//cout<<j<<endl;
				memcpy (&odata[(j)*4], &end, sizeof(int));
				}
				else{
                  outputfh.FlushPage(opagenum);
				  outputfh.UnpinPage(opagenum);
				  outputfh.FlushPages();
				  fm.ClearBuffer();
				  //cout<<"I am here"<<endl;
				  opagenum++;
				  ph2 = outputfh.NewPage ();
		          odata = ph2.GetData ();
				  j=-1;
				}
				j++;
				if(j==PAGE_CONTENT_SIZE/4){
					j=0;
					outputfh.FlushPage(opagenum);
					outputfh.UnpinPage(opagenum);
					outputfh.FlushPages();
					fm.ClearBuffer();
					//cout<<"I am here"<<endl;
					//opagenum++;
					ph2 = outputfh.NewPage ();
					odata = ph2.GetData ();
				}

	}
    //cout<<"lastpage "<<outputfh.LastPage().GetPageNum()<<endl;
    
	int end=INT_MIN;
	while(j<PAGE_CONTENT_SIZE/4){
      
	  memcpy (&odata[j*4], &end, sizeof(int));
	  j++;
	}


	fm.CloseFile (fh);
	fm.CloseFile(outputfh);
	
	cout<<"Output file"<<endl;
	fh = fm.OpenFile(argv[3]);
	print(&fh, &fm);
	fm.DestroyFile(argv[3]);
    
}