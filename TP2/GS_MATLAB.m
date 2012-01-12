function x = GS_MATLAB(A,b,iter,y)
if nargin < 4, y = 0*b; end
if nargin < 3, iter = 50; y = 0*b; end
N = size(A,1); x = b; k = 1;
while k <= iter
for ind=1:N
x(ind) = (b(ind) - A(ind,1:ind-1)*y(1:ind-1) -A(ind,ind+1:N)*x(ind+1:N))./A(ind,ind);
end
if norm(x-y,inf) < eps
return
else
y = x; k = k+1;
end
end