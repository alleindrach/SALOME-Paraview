/* Copyright 2020 NVIDIA Corporation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*  * Neither the name of NVIDIA CORPORATION nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>

#ifdef _WIN32
#else // _WIN32
#include <sys/mman.h>
#endif // _WIN32

#include "vtknvindex_forwarding_logger.h"
#include "vtknvindex_host_properties.h"
#include "vtknvindex_utilities.h"

// ------------------------------------------------------------------------------------------------
vtknvindex_host_properties::vtknvindex_host_properties()
  : m_hostid(0)
  , m_nvrankid(-1)
{
  // empty
}

// ------------------------------------------------------------------------------------------------
vtknvindex_host_properties::vtknvindex_host_properties(
  mi::Uint32 hostid, mi::Sint32 rankid, std::string hostname)
  : m_hostid(hostid)
  , m_nvrankid(rankid)
  , m_hostname(hostname)
{
  // empty
}

// ------------------------------------------------------------------------------------------------
vtknvindex_host_properties::~vtknvindex_host_properties()
{
  // empty
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::shm_cleanup(bool reset)
{
  for (auto& it : m_shmlist)
  {
    for (shm_info& current_shm : it.second)
    {
      if (current_shm.m_mapped_subset_ptr)
      {
        vtknvindex::util::unmap_shm(current_shm.m_mapped_subset_ptr, current_shm.m_size);
        current_shm.m_mapped_subset_ptr = nullptr;
      }

      // TODO: Only call unlink on the rank that created the shared memory

      // Remove shared memory (skip when local subset was available)
      if (!current_shm.m_subset_ptr)
      {
#ifdef _WIN32
// TODO: Unlink using windows functions
#else  // _WIN32
        shm_unlink(current_shm.m_shm_name.c_str());
#endif // _WIN32
      }
    }
  }

  if (reset)
    m_shmlist.clear();
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::set_shminfo(mi::Uint32 time_step, mi::Sint32 rank_id,
  std::string shmname, mi::math::Bbox<mi::Float32, 3> shmbbox, mi::Uint64 shmsize, void* subset_ptr)
{
  std::map<mi::Uint32, std::vector<shm_info> >::iterator shmit = m_shmlist.find(time_step);
  if (shmit == m_shmlist.end())
  {
    std::vector<shm_info> shmlist;
    shmlist.push_back(shm_info(rank_id, shmname, shmbbox, shmsize, subset_ptr));
    m_shmlist[time_step] = shmlist;
  }
  else
  {
    shmit->second.push_back(shm_info(rank_id, shmname, shmbbox, shmsize, subset_ptr));
  }

  // TODO : Change this to reflect the actual subcube size.
  mi::Float32 subcube_size = 510;
  mi::math::Vector_struct<mi::Float32, 3> shmvolume;
  shmvolume.x = shmbbox.max.x - shmbbox.min.x;
  shmvolume.y = shmbbox.max.y - shmbbox.min.y;
  shmvolume.z = shmbbox.max.z - shmbbox.min.z;
  mi::Float32 shm_reference_count = ceil((shmvolume.x / subcube_size)) *
    ceil((shmvolume.y / subcube_size)) * ceil((shmvolume.z / subcube_size));

  m_shmref[shmname] = static_cast<mi::Uint32>(shm_reference_count);
}

// ------------------------------------------------------------------------------------------------
bool vtknvindex_host_properties::get_shminfo(const mi::math::Bbox<mi::Float32, 3>& bbox,
  std::string& shmname, mi::math::Bbox<mi::Float32, 3>& shmbbox, mi::Uint64& shmsize,
  void** subset_ptr, mi::Uint32 time_step)
{
  std::map<mi::Uint32, std::vector<shm_info> >::iterator shmit = m_shmlist.find(time_step);
  if (shmit == m_shmlist.end())
  {
    ERROR_LOG << "The shared memory information in vtknvindex_host_properties::get_shminfo is not "
                 "available for the time step: "
              << time_step << ".";
    return false;
  }

  std::vector<shm_info>& shmlist = shmit->second;
  if (shmlist.empty())
  {
    ERROR_LOG << "The shared memory list in vtknvindex_host_properties::get_shminfo is empty.";
    return false;
  }

  for (mi::Uint32 i = 0; i < shmlist.size(); ++i)
  {
    const shm_info& current_shm = shmlist[i];

    if (current_shm.m_shm_bbox.contains(bbox.min) && current_shm.m_shm_bbox.contains(bbox.max))
    {
      shmname = current_shm.m_shm_name;
      shmbbox = current_shm.m_shm_bbox;
      shmsize = current_shm.m_size;
      *subset_ptr = current_shm.m_subset_ptr;

      return true;
    }
  }
  return false;
}

// ------------------------------------------------------------------------------------------------
vtknvindex_host_properties::shm_info* vtknvindex_host_properties::get_shminfo(
  const mi::math::Bbox<mi::Float32, 3>& bbox, mi::Uint32 time_step)
{
  std::map<mi::Uint32, std::vector<shm_info> >::iterator shmit = m_shmlist.find(time_step);
  if (shmit == m_shmlist.end())
  {
    ERROR_LOG << "The shared memory information in vtknvindex_host_properties::get_shminfo is not "
                 "available for the time step: "
              << time_step << ".";

    return NULL;
  }

  std::vector<shm_info>& shmlist = shmit->second;

  if (shmlist.empty())
  {
    ERROR_LOG << "The shared memory list in vtknvindex_host_properties::get_shminfo is empty.";
    return NULL;
  }

  for (mi::Uint32 i = 0; i < shmlist.size(); ++i)
  {
    shm_info* current_shm = &shmlist[i];

    if (current_shm->m_shm_bbox.contains(bbox.min) && current_shm->m_shm_bbox.contains(bbox.max))
    {
      return current_shm;
    }
  }
  return NULL;
}

// ------------------------------------------------------------------------------------------------
const mi::Uint8* vtknvindex_host_properties::get_subset_data_buffer(
  const mi::math::Bbox<mi::Float32, 3>& bbox, mi::Uint32 time_step,
  const vtknvindex_host_properties::shm_info** shm_info_out)
{
  vtknvindex_host_properties::shm_info* shm_info = get_shminfo(bbox, time_step);
  if (shm_info_out)
  {
    *shm_info_out = shm_info;
  }

  if (!shm_info)
  {
    return nullptr;
  }

  std::lock_guard<std::mutex> lock(m_mutex);

  if (shm_info->m_subset_ptr)
  {
    // Data is locally available in this rank, return directly
    void** subdivision_pointers = static_cast<void**>(shm_info->m_subset_ptr);
    // TODO: multiple timesteps only supported in non-MPI mode?
    return reinterpret_cast<mi::Uint8*>(subdivision_pointers[time_step]);
  }

  // Data must be in shared memory, map it if not already the case
  if (!shm_info->m_mapped_subset_ptr)
  {
    shm_info->m_mapped_subset_ptr =
      vtknvindex::util::get_vol_shm(shm_info->m_shm_name, shm_info->m_size);
  }

  return reinterpret_cast<mi::Uint8*>(shm_info->m_mapped_subset_ptr);
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::set_gpuids(std::vector<mi::Sint32> gpuids)
{
  m_gpuids = gpuids;
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::get_gpuids(std::vector<mi::Sint32>& gpuids) const
{
  gpuids = m_gpuids;
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::set_rankids(std::vector<mi::Sint32> rankids)
{
  m_rankids = rankids;
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::get_rankids(std::vector<mi::Sint32>& rankids) const
{
  rankids = m_rankids;
}

// ------------------------------------------------------------------------------------------------
const std::string& vtknvindex_host_properties::get_hostname() const
{
  return m_hostname;
}

// ------------------------------------------------------------------------------------------------
void vtknvindex_host_properties::print_info() const
{
  INFO_LOG << "Host name: " << m_hostname << " : hostid: " << m_hostid;
  INFO_LOG << "------------------";

  INFO_LOG << "Rank ids: " << m_rankids.size();
  std::cout << "        PVPLN  init info : ";
  for (mi::Uint32 i = 0; i < m_rankids.size(); ++i)
    std::cout << m_rankids[i] << ", ";
  std::cout << std::endl;

  INFO_LOG << "GPU ids: " << m_gpuids.size();
  std::cout << "        PVPLN  init info : ";
  for (mi::Uint32 i = 0; i < m_gpuids.size(); ++i)
    std::cout << m_gpuids[i] << ", ";
  std::cout << std::endl;

  INFO_LOG << "Shared memory pieces [shmname, shmbbox]";
  std::map<mi::Uint32, std::vector<shm_info> >::const_iterator shmit = m_shmlist.begin();
  for (; shmit != m_shmlist.end(); ++shmit)
  {
    const std::vector<shm_info>& shmlist = shmit->second;
    for (mi::Uint32 i = 0; i < shmlist.size(); ++i)
    {
      INFO_LOG << "Time step: " << shmit->first << " " << shmlist[i].m_shm_name << ":"
               << shmlist[i].m_shm_bbox;
    }
  }
}
