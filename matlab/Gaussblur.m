function res = Gaussblur(data, radius)

gauss_m = ones(2 * radius + 1, 2 * radius + 1);
width_g = size(gauss_m, 2);
height_g = size(gauss_m, 1);
width_d = size(data, 2);
height_d = size(data, 1);
sigma = radius / 3;
res = ones(height_d, width_d, 3);

for i = 1:radius + 1,
    for j = 1:radius + 1,
        r2 = (i - radius + 1)^2 + (j - radius + 1)^2;
        temp = 1 / (2 * pi * sigma ^ 2) * exp(r2 / (2 * sigma^2));
        gauss_m(i,j) = temp;
        gauss_m(height_g - i, j) = temp;
        gauss_m(i, width_g - j) = temp;
        gauss_m(height_g - i, width_g - j) = temp;
    end
end

for k = 1:3,
    for i = radius + 1: height_d - radius,
        for j = radius + 1: width_d - radius,
            matemp = data(i - radius:i+radius, j-radius: j+radius);
            matemp = double(matemp) .* gauss_m;
            res(i,j, k) = mean2(matemp);
        end
    end
end