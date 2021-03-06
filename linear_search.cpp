
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

	
	fh->FlushPages();

}

int main(int argc, const char* argv[]) {
	//reading query_search.txt
	ifstream inputFile;
	inputFile.open(argv[2]);
	if(!inputFile.is_open()){
	   //	cout<<"File doesn't exist";
	   return 0;
	}
	

	//opening sorted_input
	FileManager fm;
	FileHandler fh = fm.OpenFile(argv[1]);
	

	int  num;
	string str;
	vector<int> numbers;

	while (inputFile >> str >> num) {
		//cout << "Read number " << num << endl;
		numbers.push_back(num);
	}



    int lastFoundPage = fh.FirstPage().GetPageNum();
    
	FileHandler outputfh;
	outputfh=fm.CreateFile(argv[3]);
    PageHandler ph2 = outputfh.NewPage ();
	char *odata = ph2.GetData ();
	int j=0;
	int opagenum=0;

	
	for (int i = 0; i < numbers.size(); i++) {
        int search=numbers.at(i);
		num=0;
		int pagenum=0;
		PageHandler ph;
		char*data;

		while(true){
        try
        {   
            ph = fh.PageAt(pagenum);
			//fh.UnpinPage(pagenum);
        }
        catch(const InvalidPageException& e)
        {
          //  cout<<"file over"<<endl;
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
					if(j==PAGE_CONTENT_SIZE/8){
                        j=0;
                        outputfh.FlushPage(opagenum);
					    outputfh.UnpinPage(opagenum);
					    outputfh.FlushPages();
					    ph2 = outputfh.NewPage ();
					    odata = ph2.GetData ();
                        //opagenum+=1;
                    }
					//this can be done since offset and pagenum will always be in same page. (given, PAGE_CONTENT_SIZE=even multiple of 4)
					pair<int,int> p=make_pair(pagenum,i);
                    memcpy(&odata[j*8], &p,sizeof(pair<int,int>));
                    j+=1;

				//cout<<"j is : "<<j<<" ";
				// if(j<PAGE_CONTENT_SIZE/4){
				// memcpy (&odata[j*4], &pagenum, sizeof(int));
				// //cout<<j;
				// }
				// else{
                //   outputfh.FlushPage(opagenum);
				//   outputfh.UnpinPage(opagenum);
				//    outputfh.FlushPages();
				//   //fm.ClearBuffer();
				//   opagenum++;
				//   //cout<<"I am here"<<endl;
				//   ph2 = outputfh.NewPage ();
		        //   odata = ph2.GetData ();
				//   j=-1;
				// }
				// j++;
				// if(j<PAGE_CONTENT_SIZE/4){
				// //cout<<j<<endl;
				// memcpy (&odata[(j)*4], &i, sizeof(int));
				// }
				// else{
                //   outputfh.FlushPage(opagenum);
				//   outputfh.UnpinPage(opagenum);
				//   outputfh.FlushPages();
				//   //fm.ClearBuffer();
				//   opagenum++;
				//   ph2 = outputfh.NewPage ();
		        //   odata = ph2.GetData ();
				//   j=-1;
				// }
				// j++;
				// if(j==PAGE_CONTENT_SIZE/4){
				// 	j=0;
				// 	outputfh.FlushPage(opagenum);
				// 	outputfh.UnpinPage(opagenum);
				// 	outputfh.FlushPages();
				// 	//fm.ClearBuffer();
				// 	opagenum++;
				// 	ph2 = outputfh.NewPage ();
				// 	odata = ph2.GetData ();
				// }
				// cout<<"Pagenum is "<<pagenum<< " Number is "<<num<<" .Offset is "<<i<<endl;



				}
			}

        
		pagenum+=1;
        fh.UnpinPage(pagenum-1);
		fh.FlushPages();
    }



	//cout<<"Iteration "<<i<<"PagenumOutput: "<<opagenum<<"j is : "<<j<<endl;
	if(j==PAGE_CONTENT_SIZE/8){
                        j=0;
                        outputfh.FlushPage(opagenum);
					    outputfh.UnpinPage(opagenum);
					    outputfh.FlushPages();
					    ph2 = outputfh.NewPage ();
					    odata = ph2.GetData ();
                        //opagenum+=1;
                    }
					pair<int,int> p=make_pair(-1,-1);
                    memcpy(&odata[j*8], &p,sizeof(pair<int,int>));
                    j+=1;


	}
    //cout<<"lastpage "<<outputfh.LastPage().GetPageNum()<<endl;
    
	int end=INT_MIN;
	j=2*j;
	while(j<PAGE_CONTENT_SIZE/4){
      
	  memcpy (&odata[j*4], &end, sizeof(int));
	  j++;
	}

    //print(&outputfh, &fm);
	fm.CloseFile (fh);
	fm.CloseFile(outputfh);
	
	//cout<<"Output file"<<endl;
	// fh = fm.OpenFile(argv[3]);
	// print(&fh, &fm);
	// fm.DestroyFile(argv[3]);
    
}
