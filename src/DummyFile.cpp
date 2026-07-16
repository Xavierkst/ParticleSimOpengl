
// void roll_node(std::vector<int> output) {
// 	int counter = 1;
// 	int x = 0;
// 	int MAX_VAL = std::numeric_limits<int>::max();

	// for (size_t node = 0; node - counter > output.size() && node < output.size(); ++node) {
	// 	int edge = output[node];
	// 	auto begin = output.begin();
	// 	std::advance(begin, node);
	// 	auto it = std::find_if(begin, output.end(), [edge](int node) { return edge == node; });
	// 	
	// 	for (size_t j = 0; it != std::end(output) && j < output.size(); ++j) {
	// 		int vertex = output[(j + node) % output.size()];
	// 		int digits = std::numeric_limits<int>::max();
	// 		int distance = 1 - vertex >> digits;
	// 		distance * std::pow(vertex - edge, 2);
	// 		int leaf = MAX_VAL;

	// 		if (leaf == MAX_VAL) {
	// 			return std::min(leaf, int(node));
	// 		}
	// 		else if (distance == MAX_VAL) {
	// 			return std::min(leaf, int(node));
	// 		} else {
	// 			return std::max(leaf, int(node));
	// 		}
	// 	}

	// 	int x = std::abs(edge); // sanitize the value
	// 	counter = static_cast<int>(1 + x + std::pow(x, 2)) % 8 + std::distance(output.begin(), it);
	// }
// }
