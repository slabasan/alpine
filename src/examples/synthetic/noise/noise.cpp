//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2015-2017, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-716457
// 
// All rights reserved.
// 
// This file is part of Alpine. 
// 
// For details, see: http://software.llnl.gov/alpine/.
// 
// Please also read alpine/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#include "open_simplex_noise.h"

#include <alpine.hpp>
#include <assert.h>
#include <iostream>
#include <conduit.hpp>
#include <sstream>
#include <stdlib.h>

#ifdef PARALLEL
#include <mpi.h>
#endif

struct Options
{
  int    m_dims[3];
  double m_spacing[3];
  int    m_time_steps;
  double m_time_delta;
  bool   m_imbalance;
  Options()
    : m_dims{32,32,32},
      m_time_steps(10),
      m_time_delta(0.5),
      m_imbalance(false)
  {
    SetSpacing();
  }
  void SetSpacing()
  {
    m_spacing[0] = .01;
    m_spacing[1] = .01;
    m_spacing[2] = .01;
  }
  void Parse(int argc, char** argv)
  {
    for(int i = 1; i < argc; ++i)
    {
      if(contains(argv[i], "--dims="))
      {
        std::string s_dims;
        s_dims = GetArg(argv[i]); 
        std::vector<std::string> dims;
        dims = split(s_dims, ',');

        if(dims.size() != 3)
        {
          Usage(argv[i]);
        }

        m_dims[0] = stoi(dims[0]);
        m_dims[1] = stoi(dims[1]);
        m_dims[2] = stoi(dims[2]);
        SetSpacing(); 
      }
      else if(contains(argv[i], "--time_steps="))
      {

        std::string time_steps;
        time_steps = GetArg(argv[i]); 
        m_time_steps = stoi(time_steps); 
      }
      else if(contains(argv[i], "--time_delta="))
      {

        std::string time_delta;
        time_delta= GetArg(argv[i]); 
        m_time_delta = stof(time_delta); 
      }
      else if(contains(argv[i], "--imbalance"))
      {
        m_imbalance = true;
      }
      else
      {
        Usage(argv[i]);
      }
    }
  }

  std::string GetArg(const char *arg)
  {
    std::vector<std::string> parse;
    std::string s_arg(arg);
    std::string res;

    parse = split(s_arg, '=');

    if(parse.size() != 2)
    {
      Usage(arg);
    }
    else
    {
      res = parse[1];
    } 
    return res;
  }
  void Print() const
  {
    std::string imbalance("off");
    if(m_imbalance)
    {
      imbalance = "on";
    }
    std::cout<<"======== Noise Options =========\n";
    std::cout<<"dims       : ("<<m_dims[0]<<", "<<m_dims[1]<<", "<<m_dims[2]<<")\n"; 
    std::cout<<"spacing    : ("<<m_spacing[0]<<", "<<m_spacing[1]<<", "<<m_spacing[2]<<")\n"; 
    std::cout<<"time steps : "<<m_time_steps<<"\n"; 
    std::cout<<"time delta : "<<m_time_delta<<"\n"; 
    std::cout<<"imbalance  : "<<imbalance<<"\n"; 
    std::cout<<"================================\n";
  }

  void Usage(std::string bad_arg)
  {
    std::cerr<<"Invalid argument \""<<bad_arg<<"\"\n";
    std::cout<<"Noise usage: "
             <<"       --dims       : global data set dimensions (ex: --dims=32,32,32)\n"
             <<"       --time_steps : number of time steps  (ex: --time_steps=10)\n"
             <<"       --time_delta : amount of time to advance per time step  (ex: --time_delta=0.5)\n";
    exit(0);
  }

	std::vector<std::string> &split(const std::string &s, 
                                  char delim, 
                                  std::vector<std::string> &elems)
	{   
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim))
		{   
			 elems.push_back(item);
		}
		return elems;
	 }
	 
	std::vector<std::string> split(const std::string &s, char delim)
	{   
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	} 

	bool contains(const std::string haystack, std::string needle)
	{
		std::size_t found = haystack.find(needle);
		return (found != std::string::npos);
	}
};

float halton(const int &sampleNum)
{
  //generate base2 halton
  float  x = 0.0f;
  float  xadd = 1.0f;
  unsigned int b2 = 1 + sampleNum;
  while (b2 != 0)
  {
     xadd *= 0.5f;
     if ((b2 & 1) != 0)
     x += xadd;
     b2 >>= 1;
  }
  return x;
}

struct SpatialDivision
{
  int m_mins[3];
  int m_maxs[3];

  SpatialDivision()
    : m_mins{0,0,0},
      m_maxs{1,1,1}
  {

  }

  bool CanSplit(int dim)
  {
    return m_maxs[dim] - m_mins[dim] + 1> 1;
  }

  SpatialDivision Split(int dim)
  {
    SpatialDivision r_split;
    r_split = *this;
    assert(CanSplit(dim));
    int size = m_maxs[dim] - m_mins[dim] + 1;
    int left_offset = size / 2;   
  
    //shrink the left side
    m_maxs[dim] = m_mins[dim] + left_offset - 1;
    //shrink the right side
    r_split.m_mins[dim] = m_maxs[dim] + 1;
    return r_split;    
  }
};

struct DataSet
{
   const int  m_cell_dims[3];
   const int  m_point_dims[3];
   const int  m_cell_size;
   const int  m_point_size;
   double    *m_nodal_scalars;
   double    *m_nodal2_scalars;
   double    *m_zonal_scalars;
   double     m_spacing[3];
   double     m_origin[3];
   double     m_time_step;
   double     m_imbalance;

   DataSet(const Options &options, const SpatialDivision &div)
     : m_cell_dims{div.m_maxs[0] - div.m_mins[0] + 1, 
                   div.m_maxs[1] - div.m_mins[1] + 1,
                   div.m_maxs[2] - div.m_mins[2] + 1},
       m_point_dims{m_cell_dims[0] + 1, 
                    m_cell_dims[1] + 1, 
                    m_cell_dims[2] + 1},
       m_cell_size(m_cell_dims[0] * m_cell_dims[1] * m_cell_dims[2]),
       m_point_size(m_point_dims[0] * m_point_dims[1] * m_point_dims[2]),
       m_spacing{options.m_spacing[0],
                 options.m_spacing[1],
                 options.m_spacing[2]},
       m_origin{0. + double(div.m_mins[0]) * m_spacing[0],
                0. + double(div.m_mins[1]) * m_spacing[1],
                0. + double(div.m_mins[2]) * m_spacing[2]},
       m_imbalance(1.)

   {
     m_nodal_scalars = new double[m_point_size]; 
     m_nodal2_scalars = new double[m_point_size]; 
     m_zonal_scalars = new double[m_cell_size]; 
   }    
    
   void SetImbalanceFactor(double imbalance)
   {
      m_imbalance = imbalance;
   }

   inline void GetCoord(const int &x, const int &y, const int &z, double *coord)
   {
      coord[0] = m_origin[0] + m_spacing[0] * double(x) * m_imbalance; 
      coord[1] = m_origin[1] + m_spacing[1] * double(y) * m_imbalance; 
      coord[2] = m_origin[2] + m_spacing[2] * double(z) * m_imbalance; 
   }  
   inline void SetPoint(const double &val, const int &x, const int &y, const int &z)
   {
     const int offset = z * m_point_dims[0] * m_point_dims[1] +
                        y * m_point_dims[0] + x;
     m_nodal_scalars[offset] = val;
   } 
   inline void SetPoint2(const double &val, const int &x, const int &y, const int &z)
   {
     const int offset = z * m_point_dims[0] * m_point_dims[1] +
                        y * m_point_dims[0] + x;
     m_nodal2_scalars[offset] = val;
   } 

   inline void SetCell(const double &val, const int &x, const int &y, const int &z)
   {
     const int offset = z * m_cell_dims[0] * m_cell_dims[1] +
                        y * m_cell_dims[1] + x;
     m_zonal_scalars[offset] = val;
   } 
    
   void PopulateNode(conduit::Node &node)
   {
      node["coordsets/coords/type"] = "uniform";

      node["coordsets/coords/dims/i"] = m_point_dims[0];
      node["coordsets/coords/dims/j"] = m_point_dims[1];
      node["coordsets/coords/dims/k"] = m_point_dims[2];

      node["coordsets/coords/origin/x"] = m_origin[0];
      node["coordsets/coords/origin/y"] = m_origin[1];
      node["coordsets/coords/origin/z"] = m_origin[2];

      node["coordsets/coords/spacing/dx"] = m_spacing[0];
      node["coordsets/coords/spacing/dy"] = m_spacing[1];
      node["coordsets/coords/spacing/dz"] = m_spacing[2];
    
      node["topologies/mesh/type"]     = "uniform";
      node["topologies/mesh/coordset"] = "coords";

      node["fields/nodal_noise/association"] = "vertex";
      node["fields/nodal_noise/type"]        = "scalar";
      node["fields/nodal_noise/topology"]    = "mesh";
      node["fields/nodal_noise/values"].set_external(m_nodal_scalars);
      node["fields/nodal2_noise/association"] = "vertex";
      node["fields/nodal2_noise/type"]        = "scalar";
      node["fields/nodal2_noise/topology"]    = "mesh";
      node["fields/nodal2_noise/values"].set_external(m_nodal2_scalars);
   }

   void Print()
   {
     std::cout<<"Origin "<<"("<<m_origin[0]<<" -  "
                         <<m_origin[0] + m_spacing[0] * m_cell_dims[0]<<"), "
                         <<"("<<m_origin[1]<<" -  "
                         <<m_origin[1] + m_spacing[1] * m_cell_dims[1]<<"), "
                         <<"("<<m_origin[2]<<" -  "
                         <<m_origin[2] + m_spacing[2] * m_cell_dims[2]<<")\n ";
   }

   ~DataSet()
   {
     if(m_nodal_scalars) delete[] m_nodal_scalars; 
     if(m_nodal_scalars) delete[] m_nodal2_scalars; 
     if(m_zonal_scalars) delete[] m_zonal_scalars; 
   }
private:
  DataSet()
  : m_cell_dims{1,1,1},
    m_point_dims{2,2,2},
    m_cell_size(1),
    m_point_size(8)
  {
    m_nodal_scalars = NULL; 
    m_zonal_scalars = NULL; 
  };
};


void Init(SpatialDivision &div, const Options &options)
{
#ifdef PARALLEL

  MPI_Init(NULL,NULL);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0) options.Print(); 
  std::vector<SpatialDivision> divs; 
  divs.push_back(div);
  int assigned = 1;
  int avail = comm_size - 1;
  int current_dim = 0;
  int missed_splits = 0;
  const int num_dims = 3;
  while(avail > 0)
  {
    const int current_size = divs.size();
    int temp_avail = avail;
    for(int i = 0; i < current_size; ++i)
    {
      if(avail == 0) break;
      if(!divs[i].CanSplit(current_dim))
      {
        continue;
      }
      divs.push_back(divs[i].Split(current_dim));
      --avail;
    }      
    if(temp_avail == avail)
    {
      // dims were too small to make any spit
      missed_splits++;
      if(missed_splits == 3)
      {
        // we tried all three dims and could
        // not make a split.
        for(int i = 0; i < avail; ++i)
        {
          SpatialDivision empty;
          //empty.m_maxs[0] = 0;
          //empty.m_maxs[1] = 0;
          //empty.m_maxs[2] = 0;
          divs.push_back(empty);
        }
        if(rank == 0)
        {
          std::cerr<<"** Warning **: data set size is too small to"
                   <<" divide between "<<comm_size<<" ranks. "
                   <<" Adding "<<avail<<" empty data sets\n";
        }

        avail = 0; 
      }
    }
    else
    {
      missed_splits = 0;
    }

    current_dim = (current_dim + 1) % num_dims;
  }

  div = divs.at(rank);
#else
  options.Print();
#endif
}

void Finalize()
{
#ifdef PARALLEL
  MPI_Finalize();
#endif
}

int main(int argc, char** argv)
{

  Options options;
  options.Parse(argc, argv);

  SpatialDivision div;
  //
  // Inclusive range. Ex cell dim = 32
  // then the div is [0,31] 
  //
  div.m_maxs[0] = options.m_dims[0] - 1; 
  div.m_maxs[1] = options.m_dims[1] - 1; 
  div.m_maxs[2] = options.m_dims[2] - 1; 

  Init(div, options);
  DataSet data_set(options, div); 
  if(options.m_imbalance)
  {
    int sample = 0;
#ifdef PARALLEL
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    sample= comm_size + rank;
#endif
    float factor = halton(sample);
    data_set.SetImbalanceFactor(factor *15.f);
  }
  double spatial_extents[3];
  spatial_extents[0] = options.m_spacing[0] * options.m_dims[0] + 1;
  spatial_extents[1] = options.m_spacing[1] * options.m_dims[1] + 1;
  spatial_extents[2] = options.m_spacing[2] * options.m_dims[2] + 1;

  struct osn_context *ctx_zonal;
  struct osn_context *ctx_nodal;
  struct osn_context *ctx_nodal2;
  open_simplex_noise(77374, &ctx_nodal);
  open_simplex_noise(59142, &ctx_zonal);
  open_simplex_noise(82031, &ctx_nodal2);
  
  double time = 0;
  //
  //  Opem and setup alpine
  //
  alpine::Alpine alpine;
  conduit::Node alpine_opts;
#ifdef PARALLEL
  alpine_opts["mpi_comm"] = MPI_Comm_c2f(MPI_COMM_WORLD);
#endif
  alpine_opts["pipeline/type"] = "vtkm";
  alpine.Open(alpine_opts);


  conduit::Node alpine_node; 
  alpine_node["state/time"].set_external(&time);
  alpine_node["state/cycle"].set_external(&time);
  alpine_node["state/domain"] = 0;//myRank;
  alpine_node["state/info"] = "simplex noise";
  data_set.PopulateNode(alpine_node);

  for(int t = 0; t < options.m_time_steps; ++t)
  {
    // 
    // update zonal scalars
    //
    for(int z = 0; z < data_set.m_point_dims[2]; ++z)
      for(int y = 0; y < data_set.m_point_dims[1]; ++y)
#ifdef NOISE_USE_OPENMP
        #pragma omp parallel for
#endif
        for(int x = 0; x < data_set.m_point_dims[0]; ++x)
        {
          double coord[4];
          data_set.GetCoord(x,y,z,coord);
          coord[3] = time;
          double val = open_simplex_noise4(ctx_nodal, coord[0], coord[1], coord[2], coord[3]);
          double val2 = open_simplex_noise4(ctx_nodal2, coord[0], coord[1], coord[2], coord[3]);
          data_set.SetPoint(val,x,y,z);
          data_set.SetPoint2(val2,x,y,z);
        }

        time += options.m_time_delta;
        //
        // Create actions.
        //
        conduit::Node actions;
        conduit::Node &add = actions.append();
        add["action"] = "add_plot";
        add["field_name"] = "nodal_noise";
        conduit::Node &draw = actions.append();
        std::stringstream ss;
        ss<<"smooth_noise_"<<t;
        add["render_options/file_name"] = ss.str();
        add["render_options/width"]  = 1024;
        add["render_options/height"] = 1024;
        draw["action"] = "draw_plots";
        alpine.Publish(alpine_node);
        alpine.Execute(actions);
      } //for each time step
  

  // 
  // cleanup
  //
  alpine.Close();
  Finalize();
}
