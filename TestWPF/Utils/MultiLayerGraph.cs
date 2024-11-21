using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestWPF.Utils;

public class GraphLayer { }

public class MultiLayerGraph
{
    private int[,,] adjacencyMatrix;
    private int size;

    // 初始化多层有向图的矩阵
    public MultiLayerGraph(int size, int layers)
    {
        this.size = size;
        adjacencyMatrix = new int[layers, size, size];
    }

    // 添加有向边到指定层
    public void AddEdge(int layer, int from, int to)
    {
        if (layer >= 0)
        {
            adjacencyMatrix[layer, from, to] = 1; // 1 表示有边，0 表示没有边
        }
        else
        {
            throw new ArgumentOutOfRangeException(nameof(layer), "层索引超出范围");
        }
    }

    // 检查指定层中边是否存在
    public bool HasEdge(int layer, int from, int to)
    {
        if (layer >= 0)
        {
            return adjacencyMatrix[layer, from, to] == 1;
        }
        else
        {
            throw new ArgumentOutOfRangeException(nameof(layer), "层索引超出范围");
        }
    }

    // 获取某层的邻接矩阵
    public int[,] GetLayerMatrix(int layer)
    {
        if (layer >= 0)
        {
            int[,] layerMatrix = new int[size, size];
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    layerMatrix[i, j] = adjacencyMatrix[layer, i, j];
                }
            }
            return layerMatrix;
        }
        else
        {
            throw new ArgumentOutOfRangeException(nameof(layer), "层索引超出范围");
        }
    }
}
