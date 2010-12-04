String.prototype.namespace = function(separator) {
  this.split(separator || '.').inject(window, function(parent, child) {
    return parent[child] == undefined ? parent[child] = { } : parent[child];
  });
}
