/*
   Copyright (c) <2014> Verisign, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights 
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
   copies of the Software, and to permit persons to whom the Software is furnished 
   to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all 
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <stdio.h>

#include "cnt_id_cache.h"
#include "cnt_id.h"

using namespace std;

CntIdCache CntIdCache::s_oInstance;

CntIdCache::CntIdCache()
{

}

CntIdCache::~CntIdCache()
{
  clear();
}

CntIdCache &CntIdCache::getInstance()
{
  return s_oInstance;
}

bool CntIdCache::addID(CntID &p_oID, time_t p_tTTL)
{
  bool bRet = false;

  CntIdMapIter_t tIter = m_oMap.find(p_oID.getInbox());
  if (m_oMap.end() != tIter)
  {
    delete tIter->second.m_pID;
  }
  CntIdTtl_t tEntry;
  tEntry.m_tExpiration = (0 == p_tTTL) ? 0 : time(NULL) + p_tTTL;
  tEntry.m_pID = new CntID();
  (*tEntry.m_pID) = p_oID;
  m_oMap[p_oID.getEmail()] = tEntry;

  bRet = true;

  return bRet;
}

bool CntIdCache::lookupSmimeID(std::string &p_sID, CntID &p_oOutputID)
{
  bool bRet = false;

  CntIdMapIter_t tIter = m_oMap.find(p_sID);
  if (m_oMap.end() != tIter)
  {
    CntIdTtl_t tEntry = tIter->second;
    time_t tNow = time(NULL);
    if (0 < tEntry.m_tExpiration && tEntry.m_tExpiration < tNow)
    {
// cnt_log("Deleting ID: %s\n", p_sID.c_str());
      delete tEntry.m_pID;
      m_oMap.erase(tIter);
    }
    else if (NULL == tEntry.m_pID)
    {
      cnt_log("Cache ID map has a NULL pointer in ID: %s\n", p_sID.c_str());
    }
    else
    {
      p_oOutputID = *tEntry.m_pID;
      bRet = true;
    }
  }

  return bRet;
}

bool CntIdCache::clear()
{
  for (CntIdMapIter_t tIter = m_oMap.begin();
       m_oMap.end() != tIter;
       tIter++)
  {
    delete tIter->second.m_pID;
  }
  m_oMap.clear();

  return true;
}

