%% Clear
close all;
clear all;
clc;

%% Histograms

subplot(2,2,1);
make_hist(13,92952);
subplot(2,2,2);
make_hist(13,93015);
subplot(2,2,3);
make_hist(14,92952);
subplot(2,2,4);
make_hist(14,93015);
sgtitle("Random Permutations");
