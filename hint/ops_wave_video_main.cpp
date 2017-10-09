#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include "ops_wave_video.h"

void ops_wave_deriv(my_base_type *u, my_base_type *v, my_base_type *udt,
		my_base_type *vdt, my_const_base_type *c1)
{
	int idx;
	for(int j=0,idx=0;j<HEIGHT;j++)
	{
		for(int i=0;i<WIDTH;i++,idx++)
		{
			if ((j==0)||(i==0)||(j==HEIGHT-1)||(i==WIDTH-1))
			{
				udt[idx]=0.0;
				vdt[idx]=0.0;
			}
			else
			{
				udt[idx]=v[idx];
				vdt[idx]=c1[idx]*(u[idx-1]+u[idx+1]+u[idx+WIDTH]+u[idx-WIDTH]-4.0*u[idx]);
			}
		}
	}
}

void ops_wave_test(my_base_type *u, my_base_type *v, my_base_type *u2,
		my_base_type *v2, my_const_base_type *c1, my_base_type dt_2,
		my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2) {
	my_base_type *udt=new my_base_type[HEIGHT*WIDTH];
	my_base_type *vdt=new my_base_type[HEIGHT*WIDTH];
	my_base_type *utmp=new my_base_type[HEIGHT*WIDTH];
	my_base_type *vtmp=new my_base_type[HEIGHT*WIDTH];
	int idx;
	ops_wave_deriv(u,v,udt,vdt,c1);
	for(idx=0;idx<HEIGHT*WIDTH;idx++)
	{
		utmp[idx]=u[idx]+dt_2*udt[idx];
		vtmp[idx]=v[idx]+dt_2dx2*vdt[idx];
	}
	ops_wave_deriv(utmp,vtmp,udt,vdt,c1);
	for(idx=0;idx<HEIGHT*WIDTH;idx++)
	{
		u2[idx]=u[idx]+dt*udt[idx];
		v2[idx]=v[idx]+dt_dx2*vdt[idx];
	}
	delete[] udt;
	delete[] vdt;
	delete[] utmp;
	delete[] vtmp;
}


int main()
{
	my_base_type *u=new my_base_type[HEIGHT*WIDTH];
	my_base_type *v=new my_base_type[HEIGHT*WIDTH];
	my_base_type *u2=new my_base_type[HEIGHT*WIDTH];
	my_base_type *v2=new my_base_type[HEIGHT*WIDTH];
	my_base_type *u2sw=new my_base_type[HEIGHT*WIDTH];
	my_base_type *v2sw=new my_base_type[HEIGHT*WIDTH];
	my_base_type *u_tmp1=new my_base_type[HEIGHT*WIDTH];
	my_base_type *u_tmp2=new my_base_type[HEIGHT*WIDTH];
	my_base_type *v_tmp1=new my_base_type[HEIGHT*WIDTH];
	my_base_type *v_tmp2=new my_base_type[HEIGHT*WIDTH];
	my_const_base_type *c1=new my_const_base_type[HEIGHT*WIDTH];
	my_base_type min,max;
	unsigned *vdata=new unsigned[HEIGHT*WIDTH/2];
	int i,j,k;
	my_base_type a,b;
	my_base_type crand=2.0f/RAND_MAX;
	//srand(time(NULL));
	srand(1234);
	float r,sourceradius=10.0f;
	int sourcei=WIDTH/2;
	int sourcej=HEIGHT/4;
	int source=sourcej*WIDTH+sourcei;
	for(j=0,k=0;j<HEIGHT;j++)
	{
		for(i=0;i<WIDTH;i++,k++)
		{
			//u[k]=k+1;
			//v[k]=k+1;
/*			if ((i==0)||(i==WIDTH-1)||(j==0)||(j==HEIGHT-1))
			{
				u[k]=k+1;
			}
			else
			{
				u[k]=0;
			}*/
			//u[k]=crand*rand()-1.0;
/*			if (i>WIDTH/3&&i<2*WIDTH/3&&j>HEIGHT/3&&j<2*HEIGHT/3) {
				u[k]=1.0f;
				v[k]=1.0f;
			} else {
				u[k]=0.0f;
				v[k]=0.0f;
			}*/
			r=sqrt((sourcei-i)*(sourcei-i)+(sourcej-j)*(sourcej-j));
			if (r<sourceradius)
			{
				u[k]=cos(r*M_PI/(sourceradius*2.0));
				v[k]=0.0;
			} else {
				u[k]=0.0;
				v[k]=0.0;
			}
			c1[k]=1.0f;
		}
	}
	my_base_type dt=.05f;
	my_base_type dx=1.0f;
//	ops_wave_test(u,v,u2sw,v2sw,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	//2 PE
//	ops_wave_test(u,v,u_tmp1,v_tmp1,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
//	ops_wave_test(u_tmp1,v_tmp1,u2sw,v2sw,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	//4 PE
	//poisson_test(u,u_tmp1);
	//poisson_test(u_tmp1,u_tmp2);
	//poisson_test(u_tmp2,u_tmp1);
	//poisson_test(u_tmp1,u2sw);
	//ops_poisson(u,u2);
	ops_wave_test(u,v,u_tmp1,v_tmp1,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	ops_wave_test(u_tmp1,v_tmp1,u_tmp2,v_tmp2,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	ops_wave_test(u_tmp2,v_tmp2,u_tmp1,v_tmp1,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	ops_wave_test(u_tmp1,v_tmp1,u2sw,v2sw,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx));
	//8 PE
	//poisson_test(u,u_tmp1);
	//poisson_test(u_tmp1,u_tmp2);
	//poisson_test(u_tmp2,u_tmp1);
	//poisson_test(u_tmp1,u_tmp2);
	//poisson_test(u_tmp2,u_tmp1);
	//poisson_test(u_tmp1,u_tmp2);
	//poisson_test(u_tmp2,u_tmp1);
	//poisson_test(u_tmp1,u2sw);
	//ops_poisson(u,u2);
	//a=511.5;//1023.0/(max-min)
	//b=511.5;//1023.0-1023.0/(max-min)
	a=5115.0;
	b=511.5;
	ops_wave_video(u,v,u2,v2,c1,dt/2.0f,dt/2.0f/(dx*dx),dt,dt/(dx*dx),vdata,a,b,min,max);
	float range;
	range=max-min;
	a=1023.0f/(range*1.1f);
	b=-(min-range*.05f)*a;
	float du,dv;
	int retval=0;
	for(i=0;i<HEIGHT*WIDTH;i++)
	{
		du=fabs(u2[i]-u2sw[i]);
		dv=fabs(v2[i]-v2sw[i]);
		if (du>1e-6||dv>1e-6)
		{
			std::cout << "Error " << i << "," << u2[i] << "," << u2sw[i] << std::endl;
			std::cout << "Error " << i << "," << v2[i] << "," << v2sw[i] << std::endl;
			retval=1;
		}
	}
	if (!retval) std::cout << "OK." << std::endl;
	std::cout << "Min: " << min << std::endl;
	std::cout << "Max: " << max << std::endl;

	delete[] u;
	delete[] v;
	delete[] u2;
	delete[] v2;
	delete[] c1;
	delete[] u2sw;
	delete[] v2sw;

	return retval;
}

/*
void plate::derivs(double* X,double* dX,double h)
{
double Xcoef;
int i,j,k,k1,offset=m_Width*m_Height;
	Xcoef=h;
	for(j=0,k=0;j<m_Height;j++)
	{
		for(i=0;i<m_Width;i++,k++)
		{
			if ((j==0)||(i==0)||(j==m_Height-1)||(i==m_Width-1))
			{
				dX[k]=0.0;
			}
			else
			{
				dX[k]=Xcoef*X[offset+k];
			}
		}
	}
	Xcoef=(m_Width-1)*(m_Width-1)*h;
	for(j=0,k1=0;j<m_Height;j++)
	{
		for(i=0;i<m_Width;i++,k++,k1++)
		{
			if ((j==0)||(i==0)||(j==m_Height-1)||(i==m_Width-1))
			{
				dX[k]=0.0;
			}
			else
			{
				dX[k]=Xcoef*(X[k1-1]+X[k1-m_Width]-4.0*X[k1]+X[k1+1]+X[k1+m_Width]);
			}
		}
	}
}

void plate::RK2(double h)
{
int i,j,k,sub_samples;
double *k1X=new double[m_Width];
double *k2X=new double[m_Width];
double *tmpX=new double[m_Width];
	for(i=0;i<m_Width;i++)
	{
		m_X[i]=m_X0[i];
	}
	sub_samples=(int)(m_Sample_h/h);
	m_ErrX=0.0;
	for(i=0;i<m_Samples;i++)
	{
		for(j=0;j<sub_samples;j++)
		{
			derivs(m_X,k1X,h/2);
			for(k=0;k<m_Width;k++)
			{
				tmpX[k]=m_X[k]+k1X[k];
			}
			derivs(tmpX,k2X,h);
			for(k=0;k<m_Width;k++)
			{
				m_X[k]+=k2X[k];
			}
		}
		difference(i+1);
	}
	delete k1X;
	delete k2X;
	delete tmpX;
}
*/

/*void poisson_test(my_data_type *u,my_data_type *u2)
{
	int i,j,k,l,idx,tmpidx;
	my_data_type mix[KHEIGHT][KWIDTH];
	for(j=0,idx=0;j<HEIGHT;j++)
	{
		for(i=0;i<WIDTH;i++,idx++)
		{
			if ((i==0)||(i==WIDTH-1)||(j==0)||(j==HEIGHT-1))
			{
				u2[idx]=u[idx];
			} else
			{
				for(k=0;k<KHEIGHT;k++)
				{
					for(l=0;l<KWIDTH;l++)
					{
						tmpidx=idx+(k-KHEIGHT/2)*WIDTH+(l-KWIDTH/2);
						mix[k][l]=u[tmpidx];
					}
				}
				u2[idx]=poisson_kernel_stencil(mix);
			}
		}
	}
}*/
