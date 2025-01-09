using System;
using System.Collections.Generic;
using System.Diagnostics;

using MathNet.Numerics.LinearAlgebra;

namespace OCCTK.Utils;

public class Dijkstra {
	private readonly Matrix<double> adjacencyMatrix;
	private readonly int nodeCount;

	public Dijkstra( Matrix<double> matrix ) {
		adjacencyMatrix = matrix;
		nodeCount = matrix.RowCount;
	}

	public double[ ] FindShortestPath( int startNode ) {
		double[] distances = new double[nodeCount];
		bool[] visited = new bool[nodeCount];
		Array.Fill(distances, double.PositiveInfinity);
		distances[startNode] = 0;

		// 优先队列用于获取当前最近的未访问节点
		var priorityQueue = new SortedSet<(double distance, int node)>();
		priorityQueue.Add((0, startNode));

		while( priorityQueue.Count > 0 ) {
			var current = priorityQueue.Min;
			priorityQueue.Remove(current);
			int currentNode = current.node;

			if( visited[currentNode] )
				continue;

			visited[currentNode] = true;

			for( int neighbor = 0; neighbor < nodeCount; neighbor++ ) {
				if( adjacencyMatrix[currentNode, neighbor] != double.PositiveInfinity ) {
					double newDist =
						distances[currentNode] + adjacencyMatrix[currentNode, neighbor];
					if( newDist < distances[neighbor] ) {
						// 更新优先队列和最短路径
						priorityQueue.Remove((distances[neighbor], neighbor)); // 从队列中移除旧的距离
						distances[neighbor] = newDist;
						priorityQueue.Add((newDist, neighbor));
					}
				}
			}
		}

		return distances;
	}

	public void PrintDistances( double[ ] distances ) {
		for( int i = 0; i < distances.Length; i++ ) {
			Debug.WriteLine(
				$"Distance to node {i}: {( distances[i] == double.PositiveInfinity ? "∞" : distances[i].ToString( ) )}"
			);
		}
	}
}
