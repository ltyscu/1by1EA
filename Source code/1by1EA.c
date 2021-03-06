/********************************************************************************************************/
/* Y. Liu et.al, A Many-Objective Evolutionary Algorithm Using A One-by-One Selection Strategy (1by1EA).*/
/* Published on IEEE Transactions on Cybernetics, 2017.                                                 */
/* This source code of RPEA was implemented by Yiping Liu.                                              */
/* Please contact yiping0liu@gmail.com, if you have any problem.                                        */
/* Copyright 2014-2017 by Yiping Liu                                                                    */
/********************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <direct.h>
#include <time.h>
#include <conio.h>

#include "global.h"
#include "fitness.h" 
#include "random.h"    
#include "realinit.h"    
#include "mselect-1by1.h" 
#include "realcross2.h"   
#include "realmut1.h"     
#include "normalize.h"
#include "calc-convergence-ind.h"
#include "calc-distribution-ind.h"   
#include "pareto-relation.h"   
#include "1by1select.h"
#include "k-nearest.h" 
#include "corner.h" 
#include "copyind.h"
#include "putpop.h"


main()  /* */
{
	int g,i,norun;
	FILE *fp_obj,*fp_var;

	old_pop_ptr = &(oldpop);
	mate_pop_ptr = &(matepop);
	new_pop_ptr = &(newpop);

	/*-----common parameter settings-----*/	
	test_flag=2;
	nfunc=6;
	nvar=nfunc+9;
	popsize=120;
	gener=300;
	maxrun=5;
	seed0=0.0233;

	ncons=0; 
	pcross=1.00; 
	pmut_r=1.00/(1.00*nvar); 
	di=20; 
	dim=20; 
	for(i = 0; i < nvar; i++) 
	{
		lim_r[i][0]=0;
		lim_r[i][1]=1;
	}
	var_bound=1;

	/*-----1by1EA parameter settings-----*/
	switch_norm=0;
	convergence_type=0;
	corner_type=convergence_type;
	thr_zeta0=1.0;
	thr_ratio=1.0;
	nbsize=floor(popsize*0.1);

	/*-----make directories-----*/
	sprintf(dir,"dtlz%d-%d",test_flag,nfunc);
	mkdir(dir);

	strcpy(filename,dir);
	strcat(filename,"\\objective.txt");
	fp_obj=fopen(filename,"w");

	strcpy(filename,dir);
	strcat(filename,"\\variable.txt");
	fp_var=fopen(filename,"w");

	/*-----run 'maxrun' times-----*/
	for(norun=0;norun<maxrun;norun++)
	{
	printf("DTLZ%d\tObjective=%2d\tRun=%2d\n",test_flag,nfunc,norun);
	seed=seed0+norun/(maxrun*1.00); 
	warmup_random(seed);
	old_pop_ptr = &(oldpop);
	mate_pop_ptr = &(matepop);
	new_pop_ptr = &(newpop);

	printf("runing ");
	startTime = clock();
	thr_zeta[0]=thr_zeta0;
	realinit(old_pop_ptr);
	func(old_pop_ptr,popsize,test_flag);

	/*-----evolving-----*/
	for (g = 0;g < gener;g++)
	{
		if(g%(gener/10)==gener/10-1) printf("%1.f%% ",100.0*(g+1.0)/gener);

		knst(old_pop_ptr,popsize);//k-nearest
		mselect(old_pop_ptr,mate_pop_ptr );//mating select
		realcross(new_pop_ptr ,mate_pop_ptr);//crossing
		real_mutate(new_pop_ptr );//mutation
		func(new_pop_ptr,popsize,test_flag);//calculate objective values

		for(i=0;i<popsize;i++)//merge old&new pop
			copyind(mate_pop_ptr,old_pop_ptr,i,i);
		for(i=0;i<popsize;i++)
			copyind(mate_pop_ptr,new_pop_ptr,popsize+i,i);

		corner(mate_pop_ptr,2*popsize,g);//corner solutions
		calc_convergence_ind(mate_pop_ptr,2*popsize);//convergence indicator
		calc_distribution_ind(mate_pop_ptr,2*popsize);//distribution indicator
		one_by_one_select(mate_pop_ptr,old_pop_ptr,popsize*2,popsize,g);//environment selection

		func(old_pop_ptr,popsize,test_flag);
		
	}//end g

	endTime=clock();
	runtime[norun]=(endTime-startTime)/CLOCKS_PER_SEC;
	printf("time used %.3fs\n",runtime[norun]);

	putobj(old_pop_ptr,fp_obj);
	putvar(old_pop_ptr,fp_var);

	}//end norun

	fclose(fp_obj);
	fclose(fp_var);

	printf("Done!\nPlease press the AnyKey to exit...");
	getch();

}//end main
