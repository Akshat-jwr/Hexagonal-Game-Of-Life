#include<iostream>
#include<cmath>
#include<cstdlib>
#include<fstream>
#include<string>
#include<unistd.h>
using namespace std;

class Node
{
//class storing 3 variables-> 
//activity: 1 denotes alive, 0 denotes dead;
//Death denotes method of death: 1 denotes underpopulation, 2 denotes overpopulation
//count denotes number of generations passed since death
    public:
    int activity;
    int Death;
    int count;

    Node()//default constructor
    {
        activity=0;
        Death=0;
        count=0;
    }

};

int liveNeighbours(Node** grid,int i,int j,int length)
{
    if(i==length-1&&j==0)//I took an extra case while making bug fixes!
    return grid[i][1].activity+grid[i-1][0].activity+grid[i+1][0].activity;
    else if(i==length-1&&j==2*length-2)
    return grid[i][j-1].activity+grid[i-1][j-1].activity+grid[i+1][j-1].activity;
    if(i==0)//first line of the grid
    {
        if(j==0)//first element of first line
        return grid[i][j+1].activity+grid[i+1][j].activity+grid[i+1][j+1].activity;
        else if(j==length-1)//last element of first line
        {
            return grid[i][j-1].activity+grid[i+1][j].activity+grid[i+1][j+1].activity;
        }
        else//other elements of the first line
        return grid[i][j-1].activity+grid[i+1][j].activity+grid[i+1][j+1].activity+grid[i][j+1].activity;
    }
    if(i==2*length-2)//last line of the grid
    {
        if(j==0)//first element of last line
        return grid[i][j+1].activity+grid[i-1][j].activity+grid[i-1][j+1].activity;
        else if(j==length-1)//last element of last line
        {
            return grid[i][j-1].activity+grid[i-1][j].activity+grid[i-1][j+1].activity;
        }
        else//rest of the elements of last line
        return grid[i][j-1].activity+grid[i-1][j].activity+grid[i-1][j+1].activity+grid[i][j+1].activity;
    }
    if(i==length-1)//middle line - rest of the elements
    {
        return grid[i][j-1].activity+grid[i][j+1].activity+grid[i-1][j].activity+grid[i+1][j].activity+grid[i-1][j-1].activity+grid[i+1][j-1].activity;
    }
    if(j==0)//first element of rest of the lines
    {
        if(i<length)
        return grid[i][j+1].activity+grid[i-1][j].activity+grid[i+1][j].activity+grid[i+1][j+1].activity;
        else
        return grid[i][j+1].activity+grid[i+1][j].activity+grid[i-1][j].activity+grid[i-1][j+1].activity;
    }
    if(i<length)//rest of the elements covered here
    return grid[i][j+1].activity+grid[i-1][j].activity+grid[i+1][j].activity+grid[i+1][j+1].activity+grid[i][j-1].activity+grid[i-1][j-1].activity;
    return grid[i][j+1].activity+grid[i+1][j].activity+grid[i-1][j].activity+grid[i-1][j+1].activity+grid[i][j-1].activity+grid[i+1][j-1].activity;
}


void random_resurrection(int arri[],int arrj[],Node** grid,int *arrcount,int length)
{
    srand(time(0));
    int jj;
        int ii=rand()%(2*length-1);//randomly generating indices
        if(ii<=length)
        jj=rand()%(length+ii);
        else
        jj=rand()%(2*length-ii+2);
        int times=0,presence=0;//presence ensures that the random node is not one already in births list
        for(int mm=0;mm<*arrcount;mm++)
        {
            if(arri[mm]==ii&&arrj[mm]==jj)
            presence=1;
            else
            presence=0;
        }
        while((grid[ii][jj].activity!=0 || presence==1)&&(times<=1000&&grid[ii][jj].activity!=0))
        {
            //times stores number of iterations to ensure that it does not become infinite loop
            ii=rand()%(2*length-1);
            if(ii<=length)
            jj=rand()%(length+ii);
            else
            jj=rand()%(2*length-ii+2);
            times++;
            for(int mm=0;mm<*arrcount;mm++)
            {
                if(arri[mm]==ii&&arrj[mm]==jj)
                presence=1;
                else
                presence=0;
            }
        }
        arri[*arrcount]=ii;
        arrj[*arrcount]=jj;
        (*arrcount)++;   
        //cout<<ii<<","<<jj<<" resurrected to life randomly\n";
}



void births(int arri[],int arrj[],Node** grid,int *arrcount,int length)
{
    int i,j,z;
    for(i=0;i<length;i++)//upper half of grid
    {
        for(j=0;j<length+i;j++)
        {
            if(grid[i][j].activity==0)
            {
                int nbrs=liveNeighbours(grid,i,j,length);
                if(nbrs==3||grid[i][j].count==5)//birth or resurrection 
                {
                    arri[*arrcount]=i;
                    arrj[*arrcount]=j;
                    (*arrcount)++;
                    //cout<<i<<","<<j<<" resurrected\n";

                }
                else if(grid[i][j].count!=0)
                grid[i][j].count++;
            }
        }
    }
    z=length*2-1;
    for(i=length;i<2*length-1;i++)//lower half of grid
    {
        for(j=0;j<z-(i-length)-1;j++)
        {
            if(grid[i][j].activity==0)
            {
                int nbrs=liveNeighbours(grid,i,j,length);
                if(nbrs==3||grid[i][j].count==5)//birth or resurrection
                {
                    arri[*arrcount]=i;
                    arrj[*arrcount]=j;
                    (*arrcount)++;
                    //cout<<i<<","<<j<<" resurrected\n";
                }
                else if(grid[i][j].count!=0)
                grid[i][j].count++;
            }
        }
    }
}


void deaths(int brri[],int brrj[],int *brrcount,Node**grid,int length)
{
    int i,j,z;
    for(i=0;i<length;i++)//upper half of grid
    {
        for(j=0;j<length+i;j++)
        {
            if(grid[i][j].activity==1)
            {
                int nbrs=liveNeighbours(grid,i,j,length);
                if(nbrs<2&&grid[i][j].Death!=1)//killing by underpopulation
                {
                    brri[*brrcount]=i;
                    brrj[*brrcount]=j;
                    grid[i][j].Death=1;//storing method of death: 1 = underpopulation
                    (*brrcount)++;
                    //cout<<i<<","<<j<<" killed\n";
                }
                else if(nbrs>3&&grid[i][j].Death!=2)//killing by overpopulation
                {
                    brri[*brrcount]=i;
                    brrj[*brrcount]=j;
                    grid[i][j].Death=2;//storing method of death: 2 = overpopulation
                    (*brrcount)++;
                    //cout<<i<<","<<j<<" killed\n";
                }
            }
        }
    }
    z=length*2-1;
    for(i=length;i<2*length-1;i++)//lower half of grid
    {
        for(j=0;j<z-(i-length)-1;j++)
        {
            if(grid[i][j].activity==1)
            {
                int nbrs=liveNeighbours(grid,i,j,length);
                if(nbrs==2||nbrs==3)
                continue;
                if(nbrs<2&&grid[i][j].Death!=1)//killing by underpopulation
                {
                    brri[*brrcount]=i;
                    brrj[*brrcount]=j;
                    grid[i][j].Death=1;//storing method of death: 1 = underpopulation
                    (*brrcount)++;
                    //cout<<i<<","<<j<<" killed\n";
                }
                else if(nbrs>3&&grid[i][j].Death!=2)//killing by overpopulation
                {
                    brri[*brrcount]=i;
                    brrj[*brrcount]=j;
                    grid[i][j].Death=2;//storing method of death: 2 = overpopulation
                    (*brrcount)++;
                    //cout<<i<<","<<j<<" killed\n";
                }
            }
        }
    }
}


void display(Node** grid,int length,string filename)
{
    ofstream MyFile(filename);//Creating new file for each generation
    int i,j,z;
    for(i=0;i<length;i++)//upper half of grid
    {
        for(j=0;j<length-i-1;j++)
        {
            cout<<" ";
            MyFile<<" ";
        }
        for(j=0;j<length+i;j++)
        {
            if(grid[i][j].activity==1)
            {
                cout<<"O"<<" ";
                MyFile<<"O ";//writing in the text file
            }
            else 
            {
                cout<<"  ";
                MyFile<<"  ";//writing in the text file
            }
        }
        //cout<<grid[i][j].activity<<" ";//prints 1 for alive, 0 for dead
        cout<<endl;
        MyFile<<"\n";//writing in the text file
    }
    z=length*2-1;
    for(i=length;i<2*length-1;i++)//lower half of grid
    {
        for(j=0;j<i-length+1;j++)
        {
            cout<<" ";
            MyFile<<" ";//writing in the text file
        }
        for(j=0;j<z-(i-length)-1;j++)
        {
            if(grid[i][j].activity==1)
            {
                cout<<"O"<<" ";
                MyFile<<"O ";//writing in the text file
            }
            else
            {
                cout<<"  ";
                MyFile<<"  ";//writing in the text file
            }
        }
        //cout<<grid[i][j].activity<<" ";//prints 1 for alive, 0 for dead
        cout<<endl;
        MyFile<<"\n";
    }
}


void initializePopulation(Node** grid,int length)
{
    srand(time(0));
    int i;
    for(i=0;i<length;i++)//upper half of grid
    {
        grid[i]=new Node[length+i];
        for(int k=0;k<length+i;k++)
        {
            grid[i][k].activity=rand()%(2);//assigns random value for life or death
        }
    }
    int j=2*length+i-2;
    for(;i<2*length-1;i++)//lower half of grid
    {
        grid[i]=new Node[j-i];
        for(int k=0;k<j-i;k++)
        {
            grid[i][k].activity=rand()%(2);//assigns random vallue for life or death
        }
    }
}



int main()
{
    int length,gens;
    cout<<"Enter length of side"<<endl;
    cin>>length;//length of side of the hexagon
    cout<<"Enter number of generations"<<endl;
    cin>>gens;//number of generations needed
    system("clear");
    Node* grid[2*length-1];
    int i;

    initializePopulation(grid,length);//initializing the population randomly
    string filename="Generation0.txt";
    display(grid,length,filename);//displaying the grid

for(int genz=1;genz<=gens;genz++)
{
    //storing indices of births/resurrections
    int arri[length*length];
    int arrj[length*length];
    int arrcount=0;
    //storing indices of deaths
    int brri[length*length];
    int brrj[length*length];
    int brrcount=0;
    
    births(arri,arrj,grid,&arrcount,length);//Checking the births in this generation

    if(genz%4==0)//random resurrection every 4 generations
    {
        random_resurrection(arri,arrj,grid,&arrcount,length);
    }
    
    deaths(brri,brrj,&brrcount,grid,length);//checking the deaths in this generation

    for(int adds=0;adds<arrcount;adds++)//implementing the births
    {
        grid[arri[adds]][arrj[adds]].activity=1;
        grid[arri[adds]][arrj[adds]].count=0;
    }

    for(int adds=0;adds<brrcount;adds++)//implementing the deaths
    {
        grid[brri[adds]][brrj[adds]].activity=0;
        grid[brri[adds]][brrj[adds]].count=1;
    }

    //for(int kk=0;kk<=4*length;kk++)
    //cout<<"-";
    //cout<<endl;
    string files="Generation"+to_string(genz)+".txt";//Filename for each generation
    sleep(1);
    system("clear");
    display(grid,length,files);//displaying the new generation
}

}