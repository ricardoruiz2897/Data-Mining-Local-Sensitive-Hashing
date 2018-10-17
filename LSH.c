//Ricardo Ruiz
//Arusha Herath

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define DIM 4

//How many hash functions.
#define M DIM/2 

//Scaling factor.
#define W 0.3

//M combines -Used to combine the hashvalues.
#define COMBINE 10

#define NDATA 8


float *search_LSH(float data[], 
                  float query[],
                  float temp_unit_vector[M][DIM],
                  int cluster_start[], 
                  int cluster_size[], 
                  int hash_values_int[],
                  int nClusters){

    static float return_point[DIM]; //Closest point to query in cluster.
    
    printf("\n-------------\nQuery point: ");
    for(int i = 0; i<DIM; i++){
        printf("%.5f ", query[i]);
    }
    //Set unit vector. We need it to get the hash value of the query point.
    
    //Get the hash value of the query point.
    int query_hash_val[M];
    double inner_prdct;

    for(int i = 0; i < M; i++){
        inner_prdct = 0;
        for(int j = 0; j < DIM; j++){
            inner_prdct += temp_unit_vector[i][j] * query[j];
        }   
        //printf("\ninner product: %d", (int)(inner_prdct/W));
        query_hash_val[i] = (int)(inner_prdct/W);
    }

    printf("\nHash value of query:");
    for(int i=0; i<M; i++){
        printf("%d ", query_hash_val[i]);
    }

    //Make it into a M digit number.
    int query_hash_val_int;
    int tens;

    query_hash_val_int = 0;

    for(int j = 0; j < M; j++){

        //Get 10^M-j
        tens = 1;
        for(int k = j; k < M-1; k++){
            tens = COMBINE*tens; //Macro
        }

        query_hash_val_int += query_hash_val[j]*tens; 

     }

     printf("\nQuery hash value as int: %d\n", query_hash_val_int);

     int cluster_assign[nClusters];
     int temp_cluster_assign = -1;
     int count = 0;
     for(int i = 0;  i<NDATA; i++){
        if(hash_values_int[i] != temp_cluster_assign){
            temp_cluster_assign = hash_values_int[i];
            cluster_assign[count] = temp_cluster_assign;
            count++;
        }
     }

    int query_cluster = -1; 
    //Go to that cluster.
    for(int i = 0; i < nClusters; i++){
        if(query_hash_val_int == cluster_assign[i]){
            query_cluster = i;
        }
    }

    
    if(query_cluster == -1){
        printf("\n-------\nThere are no hash values for that point.\n---------\n");
        for(int i = 0; i<DIM; i++){
            return_point[i] = -1;
        }
        return return_point;
    }

    printf("Query is in cluster: %d", query_cluster);

    int dmin = 0; //Minumun distance to query
    float closest_to_query[DIM]; //Closest point to query.
    float current_point[DIM];
    int x;
    int current_sum;

    int same = 0;
    int temp1;
    int temp2;

    //Look for the point with the smallest distance.
    for(int i = cluster_start[query_cluster]; i<cluster_start[query_cluster]+(cluster_size[query_cluster]*DIM); i+=DIM){

        current_sum = 0;
        x = 0;
        same = 0;

        for(int j = i; j<(i+DIM); j++){
            
            //printf(" %.1f %.1f ", query[x], data[j]);

            if(query[x] == data[j]){
                same++;
            }

            current_sum += (query[x] - data[j]) * (query[x] - data[j]);
            current_point[x] = data[j];
            x++;
            
        }

        //Here we know that the query point is in the cluster we are looking.

        if(same == DIM){
            return query;
        }

        if(dmin == 0){
            dmin = sqrt(current_sum);
            for(int i = 0; i<DIM; i++){
                return_point[i] = current_point[i];
            }
        }else{
            if(dmin > sqrt(current_sum)){
                dmin = sqrt(current_sum);
                for(int i = 0; i<DIM; i++){
                    return_point[i] = current_point[i];
                }
            }
        }    
    }

    //Return the point.
    return return_point;
    
}


int main(void){

  float h[M][DIM]; //Hash funcitons.
  float data[NDATA*DIM];
  int hash_val[NDATA][M];
  srand(1);
  int j;
  int k;
  float sum;

  printf("Dimensions: %d\n---------\n", DIM);
  printf("M : %d\n---------\n", M);
  printf("Hash Functions.\n");
  //first initialize each hash function 
    for(int i= 0; i<M; i++){
      for(int j= 0; j<DIM;j++){
        h[i][j] = (rand() % (100 - 0 + 1)) + 0;
        printf("%.1f ", h[i][j]);
      }
      printf("\n");
    }

    //Find the unit vector of each hash function;
    for(int i= 0; i<M;i++){
      j= 0;
      sum= 0;
      while(j<DIM){
        sum += h[i][j]*h[i][j];
        j++;
      }
      j=0;
      sum = sqrt(sum);
      while(j<DIM){
        h[i][j] = (h[i][j])/sum;
        //printf("%f ", h[i][j]);
        j++;
      }
    }

    printf("\n--------\nUnit Vectors\n");
    for(int i = 0; i < M; i++){
        for(int j = 0; j < DIM; j++){
            printf("%f ", h[i][j]);
        }
        printf("\n");
    }

    //Copy to unit vector array.
    float unit_vector[M][DIM];

    for(int i = 0; i < M; i++){
        for(int j = 0; j < DIM; j++){
            unit_vector[i][j] = h[i][j];
        }
    }

    // Initialize the Ndata*DIM points;
    printf("\n-------------\n");
    printf("Data Points.\n");
    int x = 0;
    for(int i=0; i<(NDATA *DIM);i++){
        data[i] =  (rand()/(double) RAND_MAX);
        printf("%.5f ",data[i]);
        x++;
        if(x == DIM){
            printf("\n");
            x=0;
        }
    }

    //Find the hash values
    float inner_prdct;
    int hash;
    int counter= 0;
    int counter2=0;
    int i=0;
    int temp;

    for(int i=0;i <NDATA*DIM; i+=DIM){

        //Get the point start index.
        temp= i;
        counter=0;

        for(int j=0; j<M; j++) {

            k=0;
            inner_prdct = 0;

            while(k<DIM){
                inner_prdct += h[counter][k] * data[temp];
                k++;
                temp++;
            }

            hash = (int)(inner_prdct/W);
            //printf("%d ", hash);
            hash_val[counter2][j] = hash;
            if(counter!=(M-1)){
                counter++;
                temp= i;
            }

        }
        counter2++;
    }
  
    printf("\n-------------\n");
    printf("Hash Values\n");

  for(int i=0;i<NDATA;i++){
    for(int j =0;j<M;j++){
        printf("%d ",hash_val[i][j]);
    }
    printf(" ");
  }

  /*Before sorting, make the hash values a single M digits integer.
        -multiple left most times 10^M, the next one times 10^M-1. until 10^M-(M-1), and add them together.
  */

  int hash_values_int[NDATA];
  int tens;

  for(int i = 0; i < NDATA; i++){
      
      hash_values_int[i] = 0;

      for(int j = 0; j < M; j++){

        //Get 10^M-j
        tens = 1;
        for(int k = j; k < M-1; k++){
            tens = COMBINE*tens; //Macro
        }

        hash_values_int[i] += hash_val[i][j]*tens; 

      }
  }    

  printf("\n");
  for(int i = 0; i<NDATA; i++){
      printf("%d ", hash_values_int[i]);
  }

   /*
    *Sort the integers and the hash values elements and data points accordingly.
   */
   int c = 0;
   float ftemp;
   for(int i = 0; i<NDATA; i++){
        for(int j = 0; j < NDATA; j++){
            
            //Swap hash_values_int
            if(hash_values_int[i] < hash_values_int[j]){

                temp = hash_values_int[i];
                hash_values_int[i] = hash_values_int[j];
                hash_values_int[j] = temp;
            

                //Swap the data points.
                counter = i*DIM;
                counter2 = j*DIM;
                c = 0;
                while(c < DIM){
                
                    ftemp = data[counter];
                    data[counter] = data[counter2];
                    data[counter2] = ftemp;

                    counter++;
                    counter2++;
                    c++;

                }
            }            
        }
   }
  
  printf("\n");
  for(int i = 0; i<NDATA; i++){
      printf("%d ", hash_values_int[i]);
  }

  printf("\n----------------\nSorted Data Points\n");
  x = 0;
  for(int i = 0; i<NDATA*DIM; i++){
      printf("%.5f ", data[i]);
      x++;
      if(x == DIM){
          printf("\n");
          x=0;
      }
  }

  c = 0;
  int variations = 0;
  //Get variations (different clusters)
 while(c < (NDATA)){

    if((c + 1) != (DIM*NDATA)){
        if(hash_values_int[c] != hash_values_int[c+1]){
            variations++;
        }
    }

    c++;
}

    int nClusters = variations;
    printf("\n-------\nClusters: %d", variations);

    //Get cluster start and cluster size.
    int cluster_start[nClusters];
    int cluster_size[nClusters];

    /*To get cluster start and cluster size
    *We know that each point has
    */
    cluster_start[0] = 0;
    int new_index = 1;
    temp = hash_values_int[0];
    for(int i = 0; i < NDATA; i++){

        if(hash_values_int[i] != temp){
            cluster_start[new_index] = i*DIM;
            new_index++;
        }
        
    }

    printf("\n--------\nCluster Start: ");
    for(int i = 0; i < nClusters; i++){
        printf("%d ", cluster_start[i]);
    }

    //Get cluster size.
    for(int i = 0; i<nClusters; i++){
        if(i+1 != nClusters){  
            cluster_size[i] = cluster_start[i+1]/DIM - cluster_start[i]/DIM;
        }else{
            cluster_size[i] = NDATA - cluster_start[i]/DIM;
        }

    }    

    printf("\n--------\nCluster Size: ");
    for(int i = 0; i < nClusters; i++){
        printf("%d ", cluster_size[i]);
    }

    /* For the search part. */
    //Set a query point.
    float query[DIM];
    query[0] = data[28] + 0.001;
    query[1] = data[29] + 0.001;
    query[2] = data[30] + 0.001;
    query[3] = data[31] + 0.001;

    float *rpoint;
    rpoint = search_LSH(data, query, unit_vector, cluster_start, cluster_size, hash_values_int, nClusters);

    printf("\n------\nClosest point in query:\n");
    for(int i = 0; i<DIM; i++){
        printf("%.5f ", rpoint[i]);
    }

  return 0;
  
}




