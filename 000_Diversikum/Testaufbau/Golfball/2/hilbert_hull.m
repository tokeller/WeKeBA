%% Read File

m = csvread('golfb10k_raw.csv');

%% Plot raw data
figure(1)
plot(m(:,1),m(:,2));
figure(1)
title('Mehrere Einschläge auf einer Aluplatte. ');
xlabel('Zeit /[s]');
ylabel('Amplitude / [V]');

%% Hilbert
nH = hilbert(m,980);

figure(2)
plot(imag(nH(:,1)),m(:,2));
figure(1)
title('Mehrere Einschläge auf einer Aluplatte. ');
xlabel('Zeit /[s]');
ylabel('Amplitude / [V]');