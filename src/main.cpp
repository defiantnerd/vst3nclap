#include <iostream>
#include <vector>

using timestamp_t = uint32_t;

struct event {
  int data = 0;
  timestamp_t timestamp = 0;
};

struct partition {
  uint32_t index;
  uint32_t end;
};

std::vector<event> largequeue;
std::vector<partition> partitions;
std::vector<uint32_t> indexes;


// creates an index of indexes of events stored in srcq so events can be accessed in timestamp order.
void sortqueue(const std::vector<event>& srcq,
               std::vector<partition>& parts,
               std::vector<uint32_t>& tgt )
{
  
  auto npartitions = parts.size();
  
  // as long we have partitions, we must sort
  while (npartitions > 0)
  {
    size_t nextp = 0;
    // check if there partitions index > 0 have lower timestamps
    for ( size_t n = 1 ; n < npartitions ; ++n)
    {
      // is the timesamp if the currently selected partition larger than the one at n?
      if ( srcq[parts[nextp].index].timestamp > srcq[parts[n].index].timestamp)
      {
        // this is our "best" partition.
        nextp = n;
      }
    }
    
    // nextp indexes now the partition with the event of the lowest timestamp,
    // therefore we add the index of that event into the target index map and increase the
    // index on that partition
    auto ndx = parts[nextp].index++;
    tgt.push_back(ndx);
    
    // check if that partition got events left?
    if ( parts[nextp].index == parts[nextp].end )
    {
      // if yes, remove it from the partitions.
      // because we can reduce the number if iterations to determine the next partition
      if ( nextp+1 < npartitions )
      {
        // we move the last partition to the current place...
        parts[nextp] = parts[npartitions-1];
      }
      
      // remove the last partition which is either nextp or the one that replaced nextp
      parts.pop_back();
      npartitions = parts.size(); // update the number of actively scanned partitions
    }
  } // of while
}

void setup_test(std::vector<event>& srcq,
                std::vector<partition>& parts,
                std::vector<uint32_t>& tgt )
{
  
  auto numqueues = rand() % 256;
  auto numeventstotal =  numqueues + (rand() % 8192);
  
  // clearing the queues
  srcq.clear();
  parts.clear();
  tgt.clear();
  
  int average = numeventstotal / numqueues;
  
  for (int i = 0 ; i < numqueues ; ++i)
  {
    timestamp_t l = 0;
    uint32_t startindex = (uint32_t) srcq.size();
    uint32_t numevents = (uint32_t) 1+(rand() % average); // create less than the average number of events so we don't exceed the source queue, minimum 1 event
    for ( int j = 0 ; j < numevents ; ++j)
    {
      l += rand() % 10;
      srcq.push_back({(int)srcq.size(),l});
    }
    parts.push_back({startindex, startindex+numevents});
  }
  std::cout << "Test has " << srcq.size() << " events in " << parts.size() << " partitions" << std::endl;
}

bool checkqueue(const std::vector<event>& srcq, const std::vector<uint32_t>& tgt)
{
  if ( ! (srcq.size() == tgt.size() ) ) return false;
  if ( tgt.size() < 2)
    return true;
  
  for (size_t ndx = 1 ; ndx < tgt.size() ; ++ndx)
  {
    if ( srcq[tgt[ndx]].timestamp < srcq[tgt[ndx]].timestamp )
    {
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv)
{
  std::cout << "hello world!" << std::endl;
  
  
  // setting up the large queue and partitions, indexes should have the
  // same capacity as largequeue
  largequeue.reserve(8192);
  partitions.reserve(256);
  indexes.reserve(largequeue.capacity());
  
  // set up a test
  std::cout << "setting up test" << std::endl<< std::flush;
  srand(20091972);
  srand((int)clock());
  setup_test(largequeue, partitions, indexes);
  
  // sort it
  std::cout << "sorting" << std::endl << std::flush;
  auto t1 = clock();
  sortqueue(largequeue, partitions, indexes);
  auto t2 = clock();
  std::cout << "this took " << (((t2-t1)*1000000) / CLOCKS_PER_SEC) << "ns" << std::endl;
  std::cout << "checking stability" << std::endl;
  
  if ( checkqueue(largequeue, indexes) )
  {
    std::cout << "okay" << std::endl;
  }
  else
  {
    std::cout << "failed" << std::endl;
    return -1;
  }

  return 0;
}

