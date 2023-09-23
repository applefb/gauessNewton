
% 指定CSV文件的路径
csvFileName = "E:\project\SLAM十四讲\slambook2-master\slambook2-master\ch6\build\data.csv"; % 请将文件名替换为你的CSV文件名

% 读取CSV文件
data = csvread(csvFileName);

% 提取x和y数据列
x1 = data(:, 1);
y1 = data(:, 2);
x2 = data(:, 3);
y2 = data(:, 4);
x3 = data(:, 5);
y3 = data(:, 6);

% 绘制数据
figure;
hold on;
plot(x1, y1, 'b-', 'DisplayName', 'Data 1');
plot(x2, y2, 'g-', 'DisplayName', 'Data 2');
plot(x3, y3, 'r-', 'DisplayName', 'Data 3');
xlabel('X轴');
ylabel('Y轴');
title('数据图');
legend('Location', 'Best');
grid on;
hold off;



