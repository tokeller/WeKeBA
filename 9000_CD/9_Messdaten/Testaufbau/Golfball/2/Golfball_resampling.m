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

%% cut bad edges
res_c = res_n(100:end-100,2);
plot(res_c);

%% rescale
clc;
res_sc = res_c * 2047 / 1.642 -2047;
figure(1);
plot(res_sc(90:490));
[m,n] = size(res_sc);
line([0 400], [200 200], 'lineWidth',1, 'color', [1.0 0 0], 'lineStyle', '--' );
line([0 400], [-200 -200], 'lineWidth',1, 'color', [1.0 0 0], 'lineStyle', '--' );
line([0 400], [0 0], 'lineWidth',1, 'color', [0 1.0 0], 'lineStyle', '--' );

%figure(2);
%plot(res_sc);
line([189 189], [1500 -1500], 'lineWidth',1, 'color', [1.0 0 1.0], 'lineStyle', '--' );
line([219 219], [1500 -1500], 'lineWidth',1, 'color', [1.0 0 1.0], 'lineStyle', '--' );