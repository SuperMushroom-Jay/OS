## 编译gtk程序注意事项
- 编译程序时需要加上<pre>\`pkg-config --cflags --libs gtk+-2.0\`</pre>
- 例如：<pre>gcc program.c -o program \`pkg-config --cflags --libs gtk+-2.0\`</pre>
- 注意 \` 是 ~ 键上的点！！！