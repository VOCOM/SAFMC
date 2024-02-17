/**
 * @brief Payload delivery class format
*/

#include <vector>

using namespace std;

class Delivery {
public:
  typedef enum Priority {
    Low,
    Medium,
    High,
    Critical
  };


private:
  
  Priority priority;
  vector<int> waypoints;
};