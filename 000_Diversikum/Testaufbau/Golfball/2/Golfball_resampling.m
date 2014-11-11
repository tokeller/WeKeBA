%% Read File
clear all;
m = csvread('Golfball.CSV',16,0);
n = m(:,[1,3]);
clear('m');
%% 
res_n = resample(n,10000,1250000);
plot(res_n(:,2));

%% store
csvwrite('golfball_10k.csv', res_n);