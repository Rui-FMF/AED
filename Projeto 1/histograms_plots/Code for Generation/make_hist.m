function make_hist(n,seed)

filename = "histogram_"+n+"_"+seed+".txt";

Y = importdata(filename, ',');
X = 0:(n*60);
bar(X,Y,'b');
hold on

m = sum(X.*Y)/sum(Y);
var = sum(Y.*(X-m).^2)/sum(Y);
o = sqrt(var);
G = factorial(n)*(1/(sqrt(2*pi)*o))*exp(-( ((X-m).^2)/(2*(o^2)) ));

plot(X,G,'LineWidth',2);
tit = "n = " + n + " and seed = " + seed;
title(tit);
legend("Occurrences of costs","Normal distribution");
legend('boxoff')
xlabel('cost');
ylabel('occurences');
end

