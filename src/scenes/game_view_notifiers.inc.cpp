void on_new_card_on_board() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, m_card_image_paths.size() - 1);
    int random_index = distrib(gen);
    m_board.add_card(m_card_image_paths[random_index].c_str());
}
