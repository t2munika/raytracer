#include "hitrecord.hpp"

std::vector<Hit> Hit::insert_hit_in_order(std::vector<Hit> hits, Hit h) {
  std::vector<Hit> final_hits;
  unsigned int i;
  if (!hits.empty()) {
	for (i = 0; i < hits.size(); i++) {
	  if (hits.at(i).t < h.t) {
		final_hits.push_back(hits.at(i));
	  } else {
		final_hits.push_back(h);
		break;
	  }
	}
	if (i == hits.size()) {
	  final_hits.push_back(h);
	} else {
	  for (i = i; i < hits.size(); i++) {
		final_hits.push_back(hits.at(i));
	  }
	}
  } else {
	hits.push_back(h);
	final_hits = hits;
  }
  return final_hits;
}

std::vector<Hit> Hit::merge_hits(std::vector<Hit> h1, std::vector<Hit> h2) {
  unsigned int h1_count = 0;
  unsigned int h2_count = 0;
  std::vector<Hit> hits;

  while (h1_count < h1.size() && h2_count < h2.size()) {
	Hit f1 = h1.at(h1_count);
	Hit f2 = h2.at(h2_count);

	if (f1.t < f2.t) {
	  hits.push_back(f1);
	  h1_count++;
	} else {
	  hits.push_back(f2);
	  h2_count++;
	}
  }

  if (h1_count < h1.size()) {
	for(unsigned int i = h1_count; i < h1.size(); i++) {
	  hits.push_back(h1.at(i));
	}
  } else if (h2_count < h2.size()) {
	for(unsigned int i = h2_count; i < h2.size(); i++) {
	  hits.push_back(h2.at(i));
	}
  }
  return hits;
}

HitInfo HitInfo::merge_info(HitInfo h1, HitInfo h2) {
  HitInfo final_info;
  final_info.hits = Hit::merge_hits(h1.hits, h2.hits);

  std::vector< std::pair<Hit, Hit> > final_lines;
  unsigned int h1_count = 0;
  unsigned int h2_count = 0;

  while (h1_count < h1.lines.size() && h2_count < h2.lines.size()) {
	std::pair<Hit, Hit> f1 = h1.lines.at(h1_count);
	std::pair<Hit, Hit> f2 = h2.lines.at(h2_count);

	if (f1.first.t < f2.first.t) {
	  final_lines.push_back(f1);
	  h1_count++;
	} else {
	  final_lines.push_back(f2);
	  h2_count++;
	}
  }

  if (h1_count < h1.lines.size()) {
	for(unsigned int i = h1_count; i < h1.lines.size(); i++) {
	  final_lines.push_back(h1.lines.at(i));
	}
  } else if (h2_count < h2.lines.size()) {
	for(unsigned int i = h2_count; i < h2.lines.size(); i++) {
	  final_lines.push_back(h2.lines.at(i));
	}
  }
  final_info.lines = final_lines;
  return final_info;
}

Hit HitInfo::get_closest() {
  if (hits.empty()) {
	return lines.front().first;
  } else if (lines.empty()) {
	return hits.front();
  } else {
	return (hits.front().t < lines.front().first.t) ? 
	  hits.front() : lines.front().first;
  }
}

void HitInfo::clear() { 
  hits.clear();
  lines.clear(); 
}

std::vector<Hit> HitInfo::get_all_hits() {
  std::vector<Hit> h = hits;
  for (unsigned int i = 0; i < lines.size(); i++) {
	h = Hit::insert_hit_in_order(h, lines.at(i).first);
	h = Hit::insert_hit_in_order(h, lines.at(i).second);
  }
  return h;
}

std::vector<HitInfo::LineSegment> HitInfo::insert_line_in_order(
  std::vector<HitInfo::LineSegment> lines, HitInfo::LineSegment l) {

  std::vector<HitInfo::LineSegment> final_lines;
  unsigned int i;
  if (!lines.empty()) {
	for (i = 0; i < lines.size(); i++) {
	  if (lines.at(i).first.t < l.first.t) {
		final_lines.push_back(lines.at(i));
	  } else {
		final_lines.push_back(l);
		break;
	  }
	}
	if (i == lines.size()) {
	  final_lines.push_back(l);
	} else {
	  for (i = i; i < lines.size(); i++) {
		final_lines.push_back(lines.at(i));
	  }
	}
  } else {
	lines.push_back(l);
	final_lines = lines;
  }
  return final_lines;
}



