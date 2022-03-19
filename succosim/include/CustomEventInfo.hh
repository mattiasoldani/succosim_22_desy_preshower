#include <G4Types.hh>
#include <G4VUserEventInformation.hh>

#ifndef CustomEventInfo_hh
#define CustomEventInfo_hh



class CustomEventInfo : public G4VUserEventInformation
{
public:
  CustomEventInfo(G4int n);
  ~CustomEventInfo();
  
  inline void Print()const{};

  void SetNParticles(G4int n){npart=n;}
  G4int GetNParticles(){return npart;}

private:

  G4int npart;

};






#endif








