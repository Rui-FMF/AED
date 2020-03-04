%% Clear
close all;
clear all;
clc;

%% Arrays of Data

n_14 = 1:14;
n_16 = 1:16;

Brute_Force_RT = [0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.004,0.042,0.481,6.032,81.358,1189.379];
Brute_Force_MIN = [46,62,52,98,95,134,152,172,139,197,194,183,193,267];
Brute_Force_MAX = [46,72,113,167,206,249,316,333,405,472,516,568,589,658];

Branch_Bound_RT = [0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.002,0.009,0.008,0.078,3.623,3.704,20.082];
Branch_Bound_MIN = [46,62,52,98,95,134,152,172,139,197,194,183,193,267,240,262];
Branch_Bound_MAX = [46,72,113,167,206,249,316,333,405,472,516,568,589,658,680,762];

Random_RT = [0.010,0.025,0.032,0.041,0.049,0.059,0.068,0.078,0.090,0.102,0.111,0.122,0.132,0.142,0.151,0.161];
Random_MIN = [46,62,52,98,95,134,152,172,146,197,201,208,225,301,292,313];
Random_MAX = [46,72,113,167,206,249,316,333,405,465,508,539,554,609,640,701];

%% Plot Run Time
subplot(2,2,1);
plot(n_14,Brute_Force_RT,'-s');
title('Brute Force');
xlabel('n value');
ylabel('run time');

subplot(2,2,2);
plot(n_16,Branch_Bound_RT,'-ro');
title('Branch and Bound');
xlabel('n value');
ylabel('run time');

subplot(2,2,3);
plot(n_16,Random_RT,'-gx');
title('Random Permutations');
xlabel('n value');
ylabel('run time');

subplot(2,2,4);
Brute_Force_RT(15) = 1189.379;
Brute_Force_RT(16) = 1189.379;

plot(n_16,Brute_Force_RT,'-s');
hold on
plot(n_16,Branch_Bound_RT,'-ro');
plot(n_16,Random_RT,'-gx');
t="Method Comparison" + newline + "Ignore n=15,16 for BF";
title(t);
xlabel('n value');
ylabel('run time');
legend({'Brute Force','Branch and Bound','Random Permutations'},'Location','northwest');
legend('boxoff');

sgtitle('Run Time');


%% Plot Minimum Cost
subplot(2,2,1);
plot(n_14,Brute_Force_MIN,'-s');
title('Brute Force');
xlabel('n value');
ylabel('cost');

subplot(2,2,2);
plot(n_16,Branch_Bound_MIN,'-ro');
title('Branch and Bound');
xlabel('n value');
ylabel('cost');

subplot(2,2,3);
plot(n_16,Random_MIN,'-gx');
title('Random Permutations');
xlabel('n value');
ylabel('cost');

subplot(2,2,4);
Brute_Force_MIN(15) = 267;
Brute_Force_MIN(16) = 267;

plot(n_16,Brute_Force_MIN,'-s');
hold on
plot(n_16,Branch_Bound_MIN,'-ro');
plot(n_16,Random_MIN,'-gx');
t="Method Comparison" + newline + "Ignore n=15,16 for BF";
title(t);
xlabel('n value');
ylabel('cost');
legend({'Brute Force','Branch and Bound','Random Permutations'},'Location','northwest');
legend('boxoff');

sgtitle('Min Cost');


%% Plot Maximum Cost
subplot(2,2,1);
plot(n_14,Brute_Force_MAX,'-s');
title('Brute Force');
xlabel('n value');
ylabel('cost');

subplot(2,2,2);
plot(n_16,Branch_Bound_MAX,'-ro');
title('Branch and Bound');
xlabel('n value');
ylabel('cost');

subplot(2,2,3);
plot(n_16,Random_MAX,'-gx');
title('Random Permutations');
xlabel('n value');
ylabel('cost');

subplot(2,2,4);
Brute_Force_MAX(15) = 658;
Brute_Force_MAX(16) = 658;

plot(n_16,Brute_Force_MAX,'-s');
hold on
plot(n_16,Branch_Bound_MAX,'-ro');
plot(n_16,Random_MAX,'-gx');
t="Method Comparison" + newline + "Ignore n=15,16 for BF";
title(t);
xlabel('n value');
ylabel('cost');
legend({'Brute Force','Branch and Bound','Random Permutations'},'Location','northwest');
legend('boxoff');

sgtitle('Max Cost');
