Nx=8;
tridiag=diag(1*ones(1,Nx-1),-1) + diag(-2*ones(1,Nx)) + diag(1*ones(1,Nx-1),1);

tridiagper=tridiag;
tridiagper(1,Nx)=1;
tridiagper(Nx,1)=1;
tridiagper
Vec=ones(1,Nx)
plot(tridiag\(Vec.'))

tridiagper1=tridiag;
tridiagper1(1,1)=2*tridiag(1,1);
tridiagper1(Nx,Nx)=tridiag(Nx,Nx)+tridiag(1,1);
Xm=tridiagper1\V;
U=zeros(1,Nx);
U(1)=-tridiag(1,1);
U(Nx)=-1;
Zm=tridiagper1\U;
V=zeros(1,Nx);
V(1)=1
V(Nx)=-1./tridiag(1,1);
fac=(V*Xm)./(1+V*Zm)