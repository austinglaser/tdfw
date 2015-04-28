wavfiles = dir('wav/taunt*.wav');
for wavfile = wavfiles.'
    disp(wavfile.name)
    [y, fs] = audioread(['wav/', wavfile.name]);
    loudened = zeros(size(y));
    for c = 1:size(y, 2)
        loudened(:,c) = y(:,c)/max(y(:,c))*1.5;
    end
    
    audiowrite(['loudened/' wavfile.name], loudened, fs);
end