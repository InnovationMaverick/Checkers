#pragma once
#include <random>
#include <vector>

#include "../Models/Move.h"
#include "Board.h"
#include "Config.h"

const int INF = 1e9;

class Logic
{
  public:
 // Конструктор класса Logic, инициализирует указатели на объекты Board и Config
    Logic(Board *board, Config *config) : board(board), config(config)
    {
        rand_eng = std::default_random_engine (
            !((*config)("Bot", "NoRandom")) ? unsigned(time(0)) : 0);
        scoring_mode = (*config)("Bot", "BotScoringType");
        optimization = (*config)("Bot", "Optimization");
    }
    // Метод для поиска лучших ходов для заданного цвета true - белые, false - черные
    vector<move_pos> find_best_turns(const bool color)
    {
        next_best_state.clear();
        next_move.clear();

        // Здесь должна быть реализация вызова find_first_best_turn с начальными параметрами

        int cur_state = 0;
        vector<move_pos> res;
        do
        {
            res.push_back(next_move[cur_state]);
            cur_state = next_best_state[cur_state];
        } while (cur_state != -1 && next_move[cur_state].x != -1);
        return res;
    }

private:
    // Метод для применения хода и возврата новой матрицы
    vector<vector<POS_T>> make_turn(vector<vector<POS_T>> mtx, move_pos turn) const
    {
        if (turn.xb != -1)
            mtx[turn.xb][turn.yb] = 0;
        if ((mtx[turn.x][turn.y] == 1 && turn.x2 == 0) || (mtx[turn.x][turn.y] == 2 && turn.x2 == 7))
            mtx[turn.x][turn.y] += 2;
        mtx[turn.x2][turn.y2] = mtx[turn.x][turn.y];
        mtx[turn.x][turn.y] = 0;
        return mtx;
    }
    // Метод для оценки состояния доски, возвращает ценность для заданного цвета
    double calc_score(const vector<vector<POS_T>> &mtx, const bool first_bot_color) const
    {
        // color - who is max player
    // color - определяет максимального игрока
        double w = 0, wq = 0, b = 0, bq = 0; // Счетчик для белых и черных шашек
        for (POS_T i = 0; i < 8; ++i)
        {
            for (POS_T j = 0; j < 8; ++j)
            {
                w += (mtx[i][j] == 1);
                wq += (mtx[i][j] == 3);
                b += (mtx[i][j] == 2);
                bq += (mtx[i][j] == 4);
                if (scoring_mode == "NumberAndPotential")
                {
                    w += 0.05 * (mtx[i][j] == 1) * (7 - i);
                    b += 0.05 * (mtx[i][j] == 2) * (i);
                }
            }
        }
        if (!first_bot_color)
        {
            swap(b, w);
            swap(bq, wq);
        }
        if (w + wq == 0)
            return INF;
        if (b + bq == 0)
            return 0;
        int q_coef = 4; // Коэффициент для дамок

        if (scoring_mode == "NumberAndPotential")
        {
            q_coef = 5;
        }
        return (b + bq * q_coef) / (w + wq * q_coef);
    }

    double find_first_best_turn(vector<vector<POS_T>> mtx, const bool color, const POS_T x, const POS_T y, size_t state,
                                double alpha = -1)
    {
        // Логика для поиска лучшего хода в зависимости от состояния на доске
        // ...
         // Возврат лучшего найденного хода
    }
    // Метод для рекурсивного поиска лучших ходов удаленный
    double find_best_turns_rec(vector<vector<POS_T>> mtx, const bool color, const size_t depth, double alpha = -1,
                               double beta = INF + 1, const POS_T x = -1, const POS_T y = -1)
    {
        // Рекурсивная логика для поиска лучших ходов
        // ...
    }

public:
    // Метод для поиска возможных ходов для определенного цвета (игрока)
    void find_turns(const bool color)
    {
        // Вызываем защищенный метод find_turns, передавая цвет игрока и состояние доски
        find_turns(color, board->get_board());
    }

    // Метод для поиска возможных ходов из заданной позиции (x, y)
    void find_turns(const POS_T x, const POS_T y)
    {
        // Вызываем защищенный метод find_turns с координатами позиции и текущим состоянием доски
        find_turns(x, y, board->get_board());
    }


private:
    // Защищенный метод для поиска возможных ходов с учетом цвета и состояния доски
    void find_turns(const bool color, const vector<vector<POS_T>>& mtx)
    {
        vector<move_pos> res_turns; // Вектор для хранения найденных ходов
        bool have_beats_before = false; // Флаг, указывающий, были ли побития ранее

        // Проходим по каждой клетке доски
        for (POS_T i = 0; i < 8; ++i)
        {
            for (POS_T j = 0; j < 8; ++j)
            {
                // Если клетка не пустая и цвет фигуры не соответствует заданному
                if (mtx[i][j] && mtx[i][j] % 2 != color)
                {
                    // Ищем возможные ходы для текущей фигуры
                    find_turns(i, j, mtx);
                    // Проверяем, были ли побития
                    if (have_beats && !have_beats_before)
                    {
                        have_beats_before = true; // Устанавливаем флаг, если побитие произошло
                        res_turns.clear(); // Очищаем предыдущие ходы, так как есть побитие
                    }
                    // Если были побития до этого момента и снова есть побитие
                    if ((have_beats_before && have_beats) || !have_beats_before)
                    {
                        // Добавляем найденные ходы в результирующий вектор
                        res_turns.insert(res_turns.end(), turns.begin(), turns.end());
                    }
                }
            }
        }

        turns = res_turns; // Обновляем список ходов
        shuffle(turns.begin(), turns.end(), rand_eng); // Перемешиваем ходы для случайности
        have_beats = have_beats_before; // Обновляем флаг наличия побитий
    }


    // Метод для поиска возможных ходов из заданной позиции (x, y)
    void find_turns(const POS_T x, const POS_T y, const vector<vector<POS_T>>& mtx)
    {
        turns.clear(); // Очищаем список текущих ходов
        have_beats = false; // Сброс флага наличия побитий
        POS_T type = mtx[x][y]; // Определяем тип фигуры на заданной позиции

        // Проверяем возможность побитий
        switch (type)


        {
        case 1: // Если фигура белая
        case 2: // Если фигура черная
            // Проверяем возможность побитий для обычных фигур
            for (POS_T i = x - 2; i <= x + 2; i += 4) // Проверка по вертикали
            {
                for (POS_T j = y - 2; j <= y + 2; j += 4) // Проверка по горизонтали
                {
                    // Пропускаем если выход за пределы доски
                    if (i < 0 || i > 7 || j < 0 || j > 7)
                        continue;

                    // Вычисляем координаты побитой фигуры
                    POS_T xb = (x + i) / 2, yb = (y + j) / 2;
                    // Проверяем, есть ли обычная фигура для побития
                    if (mtx[i][j] || !mtx[xb][yb] || mtx[xb][yb] % 2 == type % 2)
                        continue; // Пропускаем, если условия не выполнены

                    // Добавляем ход побития в список
                    turns.emplace_back(x, y, i, j, xb, yb);
                }
            }

            break;
        default:
            // Проверяем возможность ходов для ферзей
            for (POS_T i = -1; i <= 1; i += 2) // Проход по диагоналям
            {
                for (POS_T j = -1; j <= 1; j += 2)
                {
                    POS_T xb = -1, yb = -1; // Начальные значения для побитой клетки
                    // Проверка в одном направлении до конца доски
                    for (POS_T i2 = x + i, j2 = y + j; i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1; i2 += i, j2 += j)
                    {
                        if (mtx[i2][j2]) // Если найдена фигура
                        {
                            // Проверяем, можно ли ее побить
                            if (mtx[i2][j2] % 2 == type % 2 || (mtx[i2][j2] % 2 != type % 2 && xb != -1))
                            {
                                break; // Прерываем цикл если попадется фигура одного цвета или уже была проверка на побитие
                            }
                            xb = i2; // Запоминаем позицию последней фигуры
                            yb = j2; // Запоминаем позицию последней фигуры
                        }
                        if (xb != -1 && xb != i2) // Если позиция побитой фигуры установлена
                        {
                            // Добавляем ход побития в список
                            turns.emplace_back(x, y, i2, j2, xb, yb);
                        }
                    }
                }
            }
            break;
        }
        // Проверяем наличие других возможных ходов
        if (!turns.empty()) // Если список возможных ходов не пуст
        {
            have_beats = true; // Устанавливаем флаг, указывающий на наличие побитий
            return; // Выходим из функции, так как ходы найдены
        }

        // Проверка типа фигуры для дальнейшего определения возможных ходов
        switch (type)
        {
        case 1: // Если фигура белая
        case 2: // Если фигура черная
            // Проверка возможных ходов для обычных фигур
        {
            // Определяем направление движения в зависимости от цвета
            POS_T i = ((type % 2) ? x - 1 : x + 1); // Вычисляем координату x для следующего хода
            for (POS_T j = y - 1; j <= y + 1; j += 2) // Проходим по возможным горизонтальным позициям
            {
                // Проверяем границы доски и наличие фигуры
                if (i < 0 || i > 7 || j < 0 || j > 7 || mtx[i][j])
                    continue; // Пропускаем, если координаты выходят за границы или позиция занята
                // Добавляем возможный ход в список
                turns.emplace_back(x, y, i, j); // Добавляем ход с текущими координатами
            }
            break; // Выход из блока проверки обычных фигур
        }
        default:
            // Проверка возможных ходов для ферзей
            // Проходим по всем диагоналям, позволяя передвижение
            for (POS_T i = -1; i <= 1; i += 2) // Проходим по диагоналям (вверх-вправо и вниз-влево)
            {
                for (POS_T j = -1; j <= 1; j += 2) // Проход по диагоналям (вверх-влево и вниз-вправо)
                {
                    // Проходим по всем клеткам в заданном направлении
                    for (POS_T i2 = x + i, j2 = y + j; i2 != 8 && j2 != 8 && i2 != -1 && j2 != -1; i2 += i, j2 += j)
                    {
                        if (mtx[i2][j2]) // Если на данной клетке есть фигура
                            break; // Прерываем цикл, если не можем двигаться дальше
                        // Добавляем свободные клетки как возможные ходы в список
                        turns.emplace_back(x, y, i2, j2); // Добавляем ход в список
                    }
                }
            }
            break; // Завершаем проверку на ходы для ферзей
        }
    }

  public:
      vector<move_pos> turns; // Вектор для хранения возможных ходов
      bool have_beats; // Флаг, указывающий, есть ли побития
      int Max_depth; // Максимальная глубина для рекурсивного поиска

  private:
      default_random_engine rand_eng; // Генератор случайных чисел
      string scoring_mode; // Режим оценки для бота
      string optimization; // Настройка оптимизации
      vector<move_pos> next_move; // Вектор для хранения следующего хода
      vector<int> next_best_state; // Вектор для хранения следующего лучшего состояния
      Board* board; // Указатель на объект доски
      Config* config; // Указатель на объект конфигурации
};