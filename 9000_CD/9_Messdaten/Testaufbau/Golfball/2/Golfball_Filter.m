%% Read File

m = csvread('Golfball.CSV',16,0);
n = m(:,[1,3]);
clear('m');
%% Plot raw data
plot(n(:,1),n(:,2));
figure(1)
title('Mehrere Einschläge auf einer Aluplatte. ');
xlabel('Zeit /[s]');
ylabel('Amplitude / [V]');

%% filter
nf = filter(Hlp,n(:,2));
nff = filter(Hhp,nf);

figure(12);
subplot(3,1,1);
plot(n(:,1),n(:,2));
subplot(3,1,2);
plot(n(:,1),nf);
subplot(3,1,3);
plot(n(:,1),nff);

%% analyse
N = fft(n(:,2),2048);
figure(1);
plot(real(N));
figure(2);
plot(imag(N));
figure(3);
plot(log(abs(N)));
