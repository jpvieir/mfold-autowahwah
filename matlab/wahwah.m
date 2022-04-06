%% M-FOLD AUTO WAHWAH
%% AUTOR: JOAO PAULO VIEIRA
%% DISPONIVEL EM: https://github.com/jpvieir/mfold-autowahwah
%% CODIGO PARA USO LIVRE

function wahwah
%filename = input('');
inputid = fopen('whitenoise.pcm','r');
x = fread(inputid,'short');
Fs = 16000; Fb = 80; M = 5; maxf = 5000; minf = 20;
%[x,Fs] = audioread(filename);
finc = (maxf-minf)/M;
amp = finc/2;
fosc = 1;

if M==1
    fmed = (maxf+minf)/2;
else
    fmed = (minf + minf + finc)/2;
end;

Fc = zeros(Fs,1);

for n=1:(Fs)
    Fc(n) = fmed + amp*sin(2*pi*fosc*(n-1)/Fs);
end;


xh = zeros(3*M,1); alp = zeros(3*M,1);
c = (tan(pi*Fb/Fs)-1)/(tan(pi*Fb/Fs)+1);

l=1;

for n=1:length(x)
     
     y(n)=0;
     for i=0:(M-1)
         xh(3+3*i) = x(n);
         d = -cos(2*pi*(Fc(l) + i*finc)/Fs);
         alp(3+3*i) = -c*xh(3+3*i) + d*(1-c)*xh(2+3*i) + xh(1+3*i) - d*(1-c)*alp(2+3*i) + c*alp(1+3*i);
         alp(1+3*i) = alp(2+3*i); alp(2+3*i) = alp(3+3*i); xh(1+3*i) = xh(2+3*i); xh(2+3*i) = xh(3+3*i);
         bp = (0.5)*(x(n) - alp(3+3*i));
         y(n) = y(n) + bp;
     end;
 
     l = l+1;
     if(l>Fs)
         l = 1;
     end;

end;



%filenameedit = input('Salvar o arquivo como: ');
%audiowrite(filenameedit,y,Fs);
outputid = fopen('whitenoiseWWmatlab.pcm','w');
fwrite(outputid,y,'short');
fclose(inputid);
fclose(outputid);

N = length(x);
f=((0:N-1)/N)*Fs;
X = abs(fft(x,length(x)))/length(x);
S = abs(fft(y,length(y)))/length(y);

subplot(311)
plot(f(1:floor(length(f)/2)),X(1:floor(length(S)/2))),...
%ylim([-110 0]),...
ylabel('X(f) in dB'),...
xlabel('f in Hz');
%stem(0:length(Fc)-1, Fc);
subplot(312)
plot(f(1:floor(length(f)/2)),S(1:floor(length(S)/2))),...
%ylim([-110 0]),...
ylabel('X(f) in dB (modified signal)'),...
xlabel('f in Hz');

%stem(0:length(x)-1, x);
%%subplot(313)
%%stem(0:length(yb)-1, yb);
