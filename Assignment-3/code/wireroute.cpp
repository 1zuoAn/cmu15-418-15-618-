/**
 * Parallel VLSI Wire Routing via OpenMP
 * Name 1(andrew_id 1), Name 2(andrew_id 2)
 */

#include "wireroute.h"
#include <assert.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <omp.h>
#include <math.h>
#include <algorithm>


#define T0 50000.0  // 閸掓繂顫愬〒鈺佸
#define T_end (1e-8)
#define q 0.98   // 闁偓閻忣偆閮撮弫?
#define L 500 // 濮ｅ繋閲滃〒鈺佸閺冨墎娈戞潻顓濆敩濞嗏剝鏆熼敍灞藉祮闁鹃箖鏆?
static int _argc;
static const char **_argv;

int join_way(int way[5][2],int x1, int y1, int x2, int y2){
    
    int cnt = 0;
    way[0][0] = x1;
    way[0][1] = y1;
    int len1 = abs(x2-x1);
    int len2 = abs(y2-y1);
    if(x1 == x2 || y1 == y2){
        way[1][0] = x2;
        way[1][1] = y2;
    }else{
        if((len1==1&&len2==1)||rand()&1){
        	cnt++;
            if(rand()&1){
                way[1][0] = x2;
                way[1][1] = y1;
                way[2][0] = x2;
                way[2][1] = y2;
            }else{
                way[1][0] = x1;
                way[1][1] = y2;
                way[2][0] = x2;
                way[2][1] = y2;
            }
        }else{
            cnt += 2;
	    
            if(len2==1||(len1!=1 && rand()&1)){
		if(x1<x2)    
                way[1][0] = x1 + 1 + rand()%(x2-x1-1);
		else
		    way[1][0] = x2 + 1 + rand()%(x1-x2-1);
                way[1][1] = y1;
                way[2][0] = way[1][0];
                way[2][1] = y2;
                way[3][0] = x2;
                way[3][1] = y2;
	    
            }else{
                way[1][0] = x1;
		if(y1<y2)
                way[1][1] = y1 + 1 + rand()%(y2-y1-1);
		else
		    way[1][1] = y2 + 1 + rand()%(y1-y2-1);
                way[2][0] = x2;
                way[2][1] = way[1][1];
                way[3][0] = x2;
                way[3][1] = y2;
            }
        }
    }

    return cnt;

}


void create_new(wire_t* wires, int num_of_wires)
{
    double r1 = ((double)rand())/(RAND_MAX+1.0);
    int pos1 = (int)(num_of_wires*r1); //缁楊兛绔存稉顏冩唉閸欏鍋ｉ惃鍕秴缂?
    wire_t tmp = wires[pos1];
    int cnt = wires[pos1].cnt;
    int way[5][2] = {0};
    int new_cnt = join_way(way,tmp.index[0][0],tmp.index[0][1],tmp.index[cnt+1][0],tmp.index[cnt+1][1]);
    tmp.cnt = new_cnt;
    for(int i=0;i<=1+new_cnt;i++)
    	for(int j=0;j<2;j++){
    		tmp.index[i][j] = way[i][j];
    	}
    	
    if(cnt > new_cnt){
    	for(int i=2+new_cnt;i<=1+cnt;i++)
    		for(int j=0;j<2;j++)
    		tmp.index[i][j] = 0;
    }
   wires[pos1] = tmp;
    //wires[pos1].index = way;
}
 





int init_cost(wire_t* wires,int cost[300][300], int num_of_wires, int dim_x, int dim_y){
    int sum = 0;

    for(int i = 0; i < num_of_wires; ++i){
        wire_t tmp = wires[i];
        int x1 = tmp.index[0][0];
        int y1 = tmp.index[0][1];
        for(int j = 1; j <= tmp.cnt+1; ++j){
        
          

	        int x2 = tmp.index[j][0];
	        int y2 = tmp.index[j][1];

            	
  //          	printf("%d %d %d %d\n",x1,y1,x2,y2); 
            	
            	if(x1 == x2){
                	if(y1 < y2)
                		for(int t=y1;t<y2;t++)
                    			cost[x1][t]++;
                    	else
                    		for(int t=y1;t>y2;t--)
                    			cost[x1][t]++;
            	}

            	else{
                	if(x1 < x2)
                		for(int t=x1;t<x2;t++)
                    			cost[t][y1]++;
                    	else
                    		for(int t=x1;t>x2;t--)
                    			cost[t][y1]++;
            	}
	        
            
           	 if(j == tmp.cnt+1)
                	cost[x2][y2]++;
            	
		    x1 = x2;
	        y1 = y2;
       	 }
    }

    //    for(int i=0;i<dim_x;i++){
    // 		for(int j=0;j<dim_y;j++)
    // 			printf("%d ",cost[i][j]);
    // 		printf("\n");
    // 	}
    // 	printf("\n\n");
  

    	for(int i = 0; i < dim_x; ++i){
        	for(int j = 0; j < dim_y; ++j){
            		if(cost[i][j] > 1){
                	sum += cost[i][j] - 1;
            		}
        	}
    	}

    return sum;
}


const char *get_option_string(const char *option_name,
                      const char *default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2)
    if (strcmp(_argv[i], option_name) == 0)
      return _argv[i + 1];
  return default_value;
}

int get_option_int(const char *option_name, int default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2)
    if (strcmp(_argv[i], option_name) == 0)
      return atoi(_argv[i + 1]);
  return default_value;
}

float get_option_float(const char *option_name, float default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2)
    if (strcmp(_argv[i], option_name) == 0)
      return (float)atof(_argv[i + 1]);
  return default_value;
}

static void show_help(const char *program_path) {
  printf("Usage: %s OPTIONS\n", program_path);
  printf("\n");
  printf("OPTIONS:\n");
  printf("\t-f <input_filename> (required)\n");
  printf("\t-n <num_of_threads> (required)\n");
  printf("\t-p <SA_prob>\n");
  printf("\t-i <SA_iters>\n");
}

int main(int argc, const char *argv[]) {
  using namespace std::chrono;
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::duration<double> dsec;

  auto init_start = Clock::now();
  double init_time = 0;

  _argc = argc - 1;
  _argv = argv + 1;

  const char *input_filename = get_option_string("-f", NULL);
  int num_of_threads = get_option_int("-n", 1);
  double SA_prob = get_option_float("-p", 0.1f);
  int SA_iters = get_option_int("-i", 5);

  int error = 0;

  if (input_filename == NULL) {
    printf("Error: You need to specify -f.\n");
    error = 1;
  }

  if (error) {
    show_help(argv[0]);
    return 1;
  }

  printf("Number of threads: %d\n", num_of_threads);
  printf("Probability parameter for simulated annealing: %lf.\n", SA_prob);
  printf("Number of simulated annealing iterations: %d\n", SA_iters);
  printf("Input file: %s\n", input_filename);

  FILE *input = fopen(input_filename, "r");

  if (!input) {
    printf("Unable to open file: %s.\n", input_filename);
    return 1;
  }

  int dim_x, dim_y;
  int num_of_wires;

  fscanf(input, "%d %d\n", &dim_x, &dim_y);
  fscanf(input, "%d\n", &num_of_wires);


  wire_t *wires = (wire_t *)calloc(num_of_wires, sizeof(wire_t));
  /* Read the grid dimension and wire information from file */


  

  init_time += duration_cast<dsec>(Clock::now() - init_start).count();
  printf("Initializati");

 
    srand((unsigned)time(NULL)); //閸掓繂顫愰崠鏍閺堢儤鏆熺粔宥呯摍
    double T;
    T = T0; //閸掓繂顫愬〒鈺佸

    int cost[300][300] = {0};
   // max = init(cost,num_of_wires); //閸掓繂顫愰崠鏍︾娑擃亣袙

// ------------------- init -------------------
    
    for(int i=0;i<num_of_wires;i++){         // 閸嬬 婵傚櫅 
    //  cin >> a[k] >> a[k+1] >> b[k] >> b[k+1];
    	int x1,y1,x2,y2;
    	fscanf(input, "%d %d %d %d",&x1,
    				&y1,
    				&x2, 
    				&y2);   
    // x1 y1 x2 y2
    	wires[i].index[0][0] = x1;
    	wires[i].index[0][1] = y1;
    	wires[i].index[1][0] = x2;
    	wires[i].index[1][1] = y2;
        int index_[5][2] = {0};
    	int cnt = join_way(index_,x1,y1,x2,y2);
        for(int k=0;k<=1+cnt;k++){
    	    wires[i].index[k][0] = index_[k][0];
            wires[i].index[k][1] = index_[k][1];
        }
        wires[i].cnt = cnt;


    	/*
        int x = std::min(wires[i].index[0][0],wires[i].index[1][0]);
        int y = std::max(wires[i].index[0][0],wires[i].index[1][0]);
        if(x2 > x1){
            for(int k=x;k<=y;k++){
                cost[k][y1]++;
            }
        }
        else{
            for(int k=x;k<=y;k++){
                cost[k][y2]++;
            }
        }
        x = std::min(wires[i].index[1][0],wires[i].index[1][1]);
        y = std::max(wires[i].index[1][0],wires[i].index[1][1]);
        if(y2 > y1){
            for(int k=x;k<=y;k++){
                cost[x1][k]++;
            }
        }
        else{
            for(int k=x;k<=y;k++){
                cost[x2][k]++;
            }
        }
     */   
     
    }
   
 wire_t wires0_1[num_of_wires]; // 閻劋绨穱婵嗙摠閸樼喎顫愮憴?
 wire_t wires0_2[num_of_wires];

    


    for(int k=0;k<num_of_wires;k++){
        wire_t tmp_ = wires0_1[k];
        wire_t tmp_copy = wires[k];
        tmp_.cnt = tmp_copy.cnt;

        for(int j=0;j<4;j++){
                tmp_.index[j][0] = tmp_copy.index[j][0];
                tmp_.index[j][1] = tmp_copy.index[j][1];
        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
        }
            wires0_1[k] = tmp_;
        }


    for(int k=0;k<num_of_wires;k++){
        wire_t tmp_ = wires0_2[k];
        wire_t tmp_copy = wires[k];
        tmp_.cnt = tmp_copy.cnt;

        for(int j=0;j<4;j++){
                tmp_.index[j][0] = tmp_copy.index[j][0];
                tmp_.index[j][1] = tmp_copy.index[j][1];
        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
        }
            wires0_2[k] = tmp_;
        }
// ----------------------------------------------------

 
 wire_t wires_copy[num_of_wires];
  wire_t wires_copy2[num_of_wires];
    auto compute_start = Clock::now();
    double compute_time = 0;
    int cost_copy[300][300] = {0};
    int cost_copy2[300][300] = {0};
    int cost2[300][300] = {0};
    int f0;
    int f0_1,f0_2;
 //f1娑撳搫鍨垫慨瀣掗惄顔界垼閸戣姤鏆熼崐纭风礉f2娑撶儤鏌婄憴锝囨窗閺嶅洤鍤遍弫鏉库偓纭风礉df娑撹桨绨╅懓鍛▕閸?
    double r; // 0-1娑斿妫块惃鍕閺堢儤鏆熼敍宀€鏁ら弶銉ュ枀鐎规碍妲搁崥锔藉复閸欐鏌婄憴?
    
    while(T > T_end) // 瑜版挻淇惔锔跨秵娴滃海绮ㄩ弶鐔镐刊鎼达附妞傞敍宀勨偓鈧悘顐ょ波閺?
    {

    #pragma omp parallel
        #pragma omp sections
        {
            #pragma omp section 
            {
                int f1,f2,df;    int max = 10000;
            for(int i=0;i<L;i++)
            {

            memset(cost,0,sizeof(cost));
            memset(cost_copy,0,sizeof(cost_copy));
            //   memcpy(wires_copy,wires,sizeof(wires)); // 婢跺秴鍩楅弫鎵?
            for(int k=0;k<num_of_wires;k++){
                    wire_t tmp_copy = wires_copy[k];
                    wire_t tmp_ = wires0_1[k];
                    tmp_copy.cnt = tmp_.cnt;
                
                    for(int j=0;j<4;j++){
                        tmp_copy.index[j][0] = tmp_.index[j][0];
                        tmp_copy.index[j][1] = tmp_.index[j][1];
                    }
                    wires_copy[k] = tmp_copy;

                }
            
                int max_new = 0;
                int flag = 0;       
            
                create_new(wires0_1,num_of_wires); // 娴溠呮晸閺傛媽袙
                f1 = init_cost(wires_copy,cost_copy,num_of_wires,dim_x,dim_y);
                f2 = init_cost(wires0_1,cost,num_of_wires,dim_x,dim_y);

                for(int k=0;k<dim_x;k++)
                    for(int j=0;j<dim_y;j++){
                        if(cost[k][j] > max_new)
                            max_new = cost[k][j];
                    }
                f0_1 = f2;
                df = f2 - f1;
                if(max_new > max) flag = 1;
                else max = max_new;
            
                // 娴犮儰绗呴弰鐤tropolis閸戝棗鍨?
                if(flag||df >= 0)
                {

                    r = ((double)rand())/(RAND_MAX);
                    if(flag||exp(-df/T) <= r) // 娣囨繄鏆€閸樼喐娼甸惃鍕?
                    {   f0_1 = f1;
        //            memcpy(wires,wires_copy,sizeof(wires_copy));
                    
                    for(int k=0;k<num_of_wires;k++){
                        wire_t tmp_ = wires0_1[k];
                        wire_t tmp_copy = wires_copy[k];
                        tmp_.cnt = tmp_copy.cnt;
                
                        for(int j=0;j<4;j++){
                                tmp_.index[j][0] = tmp_copy.index[j][0];
                                tmp_.index[j][1] = tmp_copy.index[j][1];
                        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
                        }
                            wires0_1[k] = tmp_;
                        }
            
            
                    }
                }
            }

            }

            #pragma omp section
            {
                int f1,f2,df;    int max = 10000;
            for(int i=0;i<L;i++)
            {

            
            memset(cost2,0,sizeof(cost2));
            memset(cost_copy2,0,sizeof(cost_copy2));
            //   memcpy(wires_copy,wires,sizeof(wires)); // 婢跺秴鍩楅弫鎵?
            for(int k=0;k<num_of_wires;k++){
                    wire_t tmp_copy = wires_copy2[k];
                    wire_t tmp_ = wires0_2[k];
                    tmp_copy.cnt = tmp_.cnt;
                
                    for(int j=0;j<4;j++){
                        tmp_copy.index[j][0] = tmp_.index[j][0];
                        tmp_copy.index[j][1] = tmp_.index[j][1];
                    }
                    wires_copy2[k] = tmp_copy;

                }
            
                int max_new = 0;
                int flag = 0;       
            
                create_new(wires0_2,num_of_wires); // 娴溠呮晸閺傛媽袙
                f1 = init_cost(wires_copy2,cost_copy2,num_of_wires,dim_x,dim_y);
                f2 = init_cost(wires0_2,cost2,num_of_wires,dim_x,dim_y);

                for(int k=0;k<dim_x;k++)
                    for(int j=0;j<dim_y;j++){
                        if(cost2[k][j] > max_new)
                            max_new = cost2[k][j];
                    }
                f0_1 = f2;
                df = f2 - f1;
                if(max_new > max) flag = 1;
                else max = max_new;
            
                // 娴犮儰绗呴弰鐤tropolis閸戝棗鍨?
                if(flag||df >= 0)
                {

                    r = ((double)rand())/(RAND_MAX);
                    if(flag||exp(-df/T) <= r) // 娣囨繄鏆€閸樼喐娼甸惃鍕?
                    {
        //            memcpy(wires,wires_copy,sizeof(wires_copy));
                    f0_2 = f1;
                    for(int k=0;k<num_of_wires;k++){
                        wire_t tmp_ = wires0_2[k];
                        wire_t tmp_copy = wires_copy2[k];
                        tmp_.cnt = tmp_copy.cnt;
                        
                        for(int j=0;j<4;j++){
                                tmp_.index[j][0] = tmp_copy.index[j][0];
                                tmp_.index[j][1] = tmp_copy.index[j][1];
                        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
                        }
                            wires0_2[k] = tmp_;
                        }
            
            
                    }
                }
            }
            }
    
        }
        T *= q; // 闂勫秵淇?
    }


    if(f0_1 > f0_2){
        f0 = f0_2;
    for(int k=0;k<num_of_wires;k++){
        wire_t tmp_ = wires[k];
        wire_t tmp_copy = wires0_2[k];
        tmp_.cnt = tmp_copy.cnt;
        
        for(int j=0;j<4;j++){
                tmp_.index[j][0] = tmp_copy.index[j][0];
                tmp_.index[j][1] = tmp_copy.index[j][1];
        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
        }
            wires[k] = tmp_;
        }

    }else{
        f0 = f0_1;
    for(int k=0;k<num_of_wires;k++){
        wire_t tmp_ = wires[k];
        wire_t tmp_copy = wires0_1[k];
        tmp_.cnt = tmp_copy.cnt;
        
        for(int j=0;j<4;j++){
                tmp_.index[j][0] = tmp_copy.index[j][0];
                tmp_.index[j][1] = tmp_copy.index[j][1];
        // printf("%d %d\n",wires_copy[k].index[j][0],wires[k].index[j][0]);
        }
            wires[k] = tmp_;
        }
    }

  compute_time += duration_cast<dsec>(Clock::now() - compute_start).count();
  printf("Computation Time: %lf.\n", compute_time);

  /* Write wires and costs to files */
   printf("Sumcost = %d\n",f0);
	
   FILE* out = fopen("result.txt", "w");
   for(int i=0;i<num_of_wires;i++){
   	for(int j=0;j<4;j++){
   	if(wires[i].index[j][0]!=0 && wires[i].index[j][1] != 0)
   		fprintf(out, "%d %d ",wires[i].index[j][0],wires[i].index[j][1]);
   	}
   	fprintf(out, "\n");
   }
   fclose(out);
  return 0;
}
