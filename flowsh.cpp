#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <set>
using namespace std;

void vectorPrint(vector<int> vctr){
    for(int i=0; i<vctr.size(); i++){
        cout << vctr[i] << " ";
    }
}

void twoDVectorPrint(vector<vector<int>> twodvector){
    for(int i=0; i<twodvector.size(); i++){
        vectorPrint(twodvector[i]);
        cout << endl;
    }
}

void dataIn(const string filename, vector<vector<int>> &durations, int number){
    ifstream inputstream;
    int machines=0;
	int jobs=0;
    string line;
    int tmp;
    vector<int> temporary;
	inputstream.open(filename);
    for(int i=0; i<number; i++){
        durations.clear();
        getline (inputstream, line);
        inputstream >> jobs >> machines;
        getline (inputstream, line); //ha szukseg van optimalis megoldasra, beolvasni eddigieket is
        getline (inputstream, line);
        for(int j=0; j<machines; j++){
            for(int k=0; k<jobs; k++){
                inputstream >> tmp;
                temporary.push_back(tmp);
            }
            durations.push_back(temporary);
            temporary.clear();
        }
        getline (inputstream, line);
    }
	
	inputstream.close();
}

int totalTime(vector<int> vctr, vector<vector<int>> durations){
    vector<int> stoptime;
    stoptime.push_back(durations[0][vctr[0]]);
    for(int i=1; i<durations.size(); i++){ //durations.size()=gepek szama
        stoptime.push_back(0);
        stoptime[i]=stoptime[i-1]+durations[i][vctr[0]];
    }
    for(int i=1; i<vctr.size(); i++){ //vctr.size()=munkak szama
		for(int j=0; j<durations.size(); j++){
			if(j==0){
				stoptime[j]=stoptime[j]+durations[j][vctr[i]];
			} else{
				stoptime[j]=(stoptime[j-1] > stoptime[j] ? stoptime[j-1] : stoptime[j])+durations[j][vctr[i]];
			}
		}
	}
    //cout << stoptime[durations.size()-1] << " " ;
    return stoptime[durations.size()-1];
}

struct compByTime {
    bool operator() (const pair<vector<int>, int>& lhs, const pair<vector<int>, int>& rhs) const {
        bool lhsIsGreater=false;
        if(lhs.second != rhs.second){
            return lhs.second < rhs.second;
        }
        for(int i=0; i<lhs.first.size() && !lhsIsGreater; i++){
            if(lhs.first[i]>rhs.first[i] ){
                lhsIsGreater=true;
            }
        }
        return lhsIsGreater;
        //return lhs.first > rhs.first;
    }
};

void generatePermutations(set<pair<vector<int>, int>, compByTime> &permutations, vector<vector<int>> durations, int size){
    vector<int> perm;
    int index, tmp;
    int jobs=durations[0].size();
    for(int i=0; i<jobs; i++){ //perm elemei 0-tol jobs-1ig feltoltve
        perm.push_back(i);
    }
    while(permutations.size()!=size){
        for(int i=0; i<jobs; i++){ //perm elemeinek osszekeverese
            index=rand()%jobs;
            tmp=perm[i];
            perm[i]=perm[index];
            perm[index]=tmp;
        }
        tmp=totalTime(perm, durations); //permhez tartozo fitnesz ertek kiszamitasa
        permutations.insert(make_pair(perm, tmp));
    }
}

vector<int> mutation(vector<int> perm){
    int index1, index2, tmp;
    vector<int> out=perm;
    do {
        index1=rand()%perm.size();
        index2=rand()%perm.size();
    } while(index1==index2);
    tmp=out[index1];
    out[index1]=out[index2];
    out[index2]=tmp;
    return out;
}

vector<int> transfer(vector<int> perm){
    return perm;
}

void oneRoundAlgorithm(set<pair<vector<int>, int>, compByTime> &permutations, vector<vector<int>> durations, int newgennum){
    int size=permutations.size();
    auto it = permutations.begin();
    int toBeDeleted, random;
    vector<int> temp;
    /*temp=mutation((*permutations.begin()).first);
    permutations.insert(make_pair(temp, totalTime(temp, durations)));*/
    while (permutations.size() != size+newgennum){
        if(rand()%100 > 65){ //35 szazalekban valasztunk a jobbakbol (elso fele)
            random = rand()%(permutations.size()/2); //annyiadik elemet kivalasztjuk a permutationsnek
        } else {
            random = rand()%permutations.size();
        }
        advance(it, random);
        temp=(*it).first;
        temp=mutation(temp);
        permutations.insert(make_pair(temp, totalTime(temp, durations)));
        it=permutations.begin();
    }
    //generatePermutations(permutations, durations, size+newgennum/2);
    while(permutations.size()!=size){
        it=permutations.begin();
        toBeDeleted=rand()%(permutations.size()-size+400) + size-400; //utolso 400 kozul 300 veletlenszeruen mindenkepp marad - mas legyen?
        //cout << toBeDeleted << " " << permutations.size() << endl;
        advance(it, toBeDeleted);
        permutations.erase(it);
        toBeDeleted--;
    }
}

void findOptimum(set<pair<vector<int>, int>, compByTime> &permutations, vector<vector<int>> durations, int size, int newgennum, int limit){
    int count=0;
    srand(time(0));
    generatePermutations(permutations, durations, size);
    while((*permutations.begin()).second > limit ){ // && count < 2000 visszairni
        oneRoundAlgorithm(permutations, durations, newgennum);
        cout << count << " ";
        count++;
    }


}

int main(){
    const string filename="1tai20_5.txt";
    int number=2; //hanyadik adat a fajlbol
    set<pair<vector<int>, int>, compByTime> permutations; //permutaciok - idotartamuk tarolasa
    vector<vector<int>> durations; //gepekhez, munkakhoz tartozo idotartamok
    ofstream myfile; //output file

    dataIn(filename, durations, number);
    twoDVectorPrint(durations);
    findOptimum(permutations, durations, 1000, 100, 1359 ); //eredeti pop. merete, uj generacio egyedszama, elerendo ertek
    cout << "Kesz" << endl;

    /*for(auto it=permutations.begin(); it!=permutations.end(); it++){
        cout << (*permutations.begin()).second << "-";
        //vectorPrint((*permutations.begin()).first);

    }*/
    vectorPrint((*permutations.begin()).first);
    cout << (*permutations.begin()).second <<endl;


    /*myfile.open ("results.txt", ios_base::app);
    myfile << (*permutations.begin()).second << endl;
    for(int i=0; i<(*permutations.begin()).first.size(); i++){
        myfile << (*permutations.begin()).first[i]<<" ";
    }
    myfile << endl;
    myfile.close();*/

    return 0;
}